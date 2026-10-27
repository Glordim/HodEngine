#pragma once
#include "HodEngine/Renderer/Export.hpp"

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
	class FontResource;

	/// @brief The fonts a text is drawn with: a font, then its fallbacks, each followed by its own (depth first). Each font once: a cycle is cut.
	/// The faces for TextShaper::Shape, and the fonts for TextMesh::Build, in the same order. Keeps its fonts alive, their caches with them.
	class HOD_RENDERER_API FontChain final
	{
	public:

		// The fonts not loaded are left out, font included: empty then. True when it changed since the last time: a font in it or not,
		// its face (reloaded), how its glyphs are drawn. What is shaped and built with it is to be again then
		bool										Update(const std::shared_ptr<FontResource>& font);
		void										Clear();

		bool										Empty() const;
		const Vector<std::shared_ptr<FontResource>>&	GetFonts() const;
		ArrayView<const FontFace*>					GetFaces() const;
		ArrayView<TextMesh::Font>					GetMeshFonts() const;

	private:

		static void									Append(const std::shared_ptr<FontResource>& font, Vector<std::shared_ptr<FontResource>>& fonts);

	private:

		Vector<std::shared_ptr<FontResource>>		_fonts;
		Vector<const FontFace*>						_faces;
		Vector<TextMesh::Font>						_meshFonts;
		Vector<uint32_t>							_faceIds; // to tell a face reloaded

		Vector<std::shared_ptr<FontResource>>		_updatedFonts; // kept from one Update to the next
	};
}
