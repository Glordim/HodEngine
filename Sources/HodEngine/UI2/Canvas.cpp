#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Canvas.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/DrawContext.hpp"

#include <HodEngine/Core/Document/DocumentNode.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Math/Rect.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(CanvasScaler::Mode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Fit, "Fit");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Expand, "Expand");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::Shrink, "Shrink");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::MatchWidth, "MatchWidth");
		reflectionDescriptor.AddEnumValue(CanvasScaler::Mode::MatchHeight, "MatchHeight");
	}

	DESCRIBE_REFLECTED_CLASS(CanvasScaler, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &CanvasScaler::_designResolution, "DesignResolution");
		AddPropertyT(reflectionDescriptor, &CanvasScaler::_mode, "Mode");
	}

	/// @brief
	/// @param targetResolution
	/// @param outLayoutSize
	/// @param outScale
	void CanvasScaler::Compute(const Vector2& targetResolution, Vector2& outLayoutSize, float& outScale) const
	{
		outLayoutSize = _designResolution;
		outScale = 1.0f;

		if (_designResolution.GetX() <= 0.0f || _designResolution.GetY() <= 0.0f || targetResolution.GetX() <= 0.0f || targetResolution.GetY() <= 0.0f)
		{
			return;
		}

		float scaleX = targetResolution.GetX() / _designResolution.GetX();
		float scaleY = targetResolution.GetY() / _designResolution.GetY();

		switch (_mode)
		{
			case Mode::Fit: outScale = Min(scaleX, scaleY); return; // The root rect stays the design resolution
			case Mode::Expand: outScale = Min(scaleX, scaleY); break;
			case Mode::Shrink: outScale = Max(scaleX, scaleY); break;
			case Mode::MatchWidth: outScale = scaleX; break;
			case Mode::MatchHeight: outScale = scaleY; break;
		}

		outLayoutSize = targetResolution / outScale;
	}

	Canvas::Canvas()
	{
		_rootNode = DefaultAllocator::GetInstance().New<Node>();
		_rootNode->SetName("Root");
		UpdateScaling();
		_rootNode->SetDesiredSize(_layoutSize);
	}

	Canvas::~Canvas()
	{
		DefaultAllocator::GetInstance().Delete(_rootNode);
	}

	void Canvas::AddChild(Node* node)
	{
		_rootNode->AddChild(node);
	}

	void Canvas::AddChild(Node* node, LayoutParams* layoutParams)
	{
		_rootNode->AddChild(node, layoutParams);
	}

	Node* Canvas::GetRootNode() const
	{
		return _rootNode;
	}

	const CanvasScaler& Canvas::GetScaler() const
	{
		return _scaler;
	}

	void Canvas::SetScaler(const CanvasScaler& scaler)
	{
		if (_scaler != scaler)
		{
			_scaler = scaler;
			UpdateScaling();
		}
	}

	const Vector2& Canvas::GetTargetResolution() const
	{
		return _targetResolution;
	}

	void Canvas::SetTargetResolution(const Vector2& targetResolution)
	{
		if (_targetResolution != targetResolution)
		{
			_targetResolution = targetResolution;
			UpdateScaling();
		}
	}

	const Vector2& Canvas::GetLayoutSize() const
	{
		return _layoutSize;
	}

	float Canvas::GetScaleFactor() const
	{
		return _scaleFactor;
	}

	/// @brief Recomputes the root rect and scale from the scaler and the target resolution, and has the
	/// whole tree laid out again on the next UpdateLayout() when the root rect changed.
	void Canvas::UpdateScaling()
	{
		Vector2 previousLayoutSize = _layoutSize;

		Vector2 targetResolution = (_targetResolution.GetX() > 0.0f && _targetResolution.GetY() > 0.0f) ? _targetResolution : _scaler._designResolution;
		_scaler.Compute(targetResolution, _layoutSize, _scaleFactor);

		if (_layoutSize != previousLayoutSize)
		{
			_rootNode->MarkMeasureAsDirty();
			_rootNode->MarkArrangeAsDirty();
		}
	}

	bool Canvas::SerializeInDocument(DocumentNode& documentNode)
	{
		if (_rootNode->SerializeInDocument(documentNode) == false)
		{
			return false;
		}

		return Serializer::Serialize(_scaler, documentNode.AddChild("Scaler"));
	}

	bool Canvas::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		if (_rootNode->DeserializeFromDocument(documentNode) == false)
		{
			return false;
		}

		// Absent from prefabs saved before the scaler existed: they keep the defaults.
		const DocumentNode* scalerNode = documentNode.GetChild("Scaler");
		if (scalerNode != nullptr)
		{
			CanvasScaler scaler;
			if (Serializer::Deserialize(scaler, *scalerNode) == false)
			{
				return false;
			}
			SetScaler(scaler);
		}

		return true;
	}

	void Canvas::RemoveChild(Node* node)
	{
		_rootNode->RemoveChild(node);
	}

	void Canvas::UpdateLayout()
	{
		_rootNode->Measure(_layoutSize);

		Rect finalRect;
		finalRect._position = _layoutSize * -0.5f;
		finalRect._size = _layoutSize;
		_rootNode->Arrange(finalRect);
	}

	void Canvas::PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix)
	{
		DrawContext drawContext(renderView, queueType, baseMatrix);
		_rootNode->PushRenderCommand(drawContext);
	}

	Matrix4 Canvas::ComputeScaleMatrix() const
	{
		return Matrix4::Scale(Vector2(_scaleFactor, _scaleFactor));
	}
}
