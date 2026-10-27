#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/Resource/FontResource.hpp"
#include "HodEngine/Renderer/Text/TextMesh.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <memory>

namespace hod::inline text
{
	class FontFace;
}

namespace hod::inline renderer
{
	class FontFaceResource;

	/// @brief The faces a text is drawn with, for a style: the face of the font for it (see FontResource::FindStyle), then its Regular face for what
	/// that one lacks, then the faces of its fallbacks for the style, each followed by its own fallbacks (depth first). Each font and face once: a cycle is cut.
	/// The faces for TextShaper::Shape, and the fonts for TextMesh::Build, in the same order. Keeps its fonts and faces alive, their caches with them.
	class HOD_RENDERER_API FontChain final
	{
	public:

		// The ones not loaded are left out: empty without any. True when it changed since the last time: a face in it or not,
		// reloaded (another FontFace), drawn otherwise. What is shaped and built with it is to be again then
		bool										Update(const std::shared_ptr<FontResource>& font, FontResource::Weight weight, bool italic);
		bool										Update(const std::shared_ptr<FontFaceResource>& face); // the face alone
		void										Clear();

		bool										Empty() const;
		const Vector<std::shared_ptr<FontFaceResource>>&	GetFaceResources() const;
		ArrayView<const FontFace*>					GetFaces() const;
		ArrayView<TextMesh::Font>					GetMeshFonts() const;

	private:

		bool										UpdateFaces(); // from _updatedFaces
		static void									Append(const std::shared_ptr<FontResource>& font, FontResource::Weight weight, bool italic,
		                                                   Vector<std::shared_ptr<FontResource>>& fonts, Vector<std::shared_ptr<FontFaceResource>>& faces);
		static void									AppendFace(const UID& face, Vector<std::shared_ptr<FontFaceResource>>& faces);

	private:

		Vector<std::shared_ptr<FontFaceResource>>	_faceResources;
		Vector<const FontFace*>						_faces;
		Vector<TextMesh::Font>						_meshFonts;
		Vector<uint32_t>							_faceIds; // to tell a face reloaded
		Vector<std::shared_ptr<FontResource>>		_fonts;   // the font and its fallbacks, kept loaded

		Vector<std::shared_ptr<FontFaceResource>>	_updatedFaces; // kept from one Update to the next
		Vector<std::shared_ptr<FontResource>>		_updatedFonts; // same
	};
}
