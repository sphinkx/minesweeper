#pragma once

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Material.h>

namespace minesweeper
{
	using namespace Urho3D;

	enum CellMaterial
	{
		Ordinal,
		Pushed,
		Suspicion,
		Bomb
	};

	// represent view for single cell. For logic see `Cell.h`
	class CellComponent : public LogicComponent
	{
		URHO3D_OBJECT(CellComponent, LogicComponent);

	public:
		explicit CellComponent(Context* context);

	#pragma region coords
		// place cell on screen
		void SetCoord(unsigned int row_, unsigned int col_);

	private:
		unsigned int row;
		unsigned int col;
	public:
		unsigned int GetRow() { return row; }
		unsigned int GetCol() { return col; }

	#pragma endregion

	#pragma region Material and number overlay
	public:
		static Material *ordinal;
		static Material *pushed;
		static Material *pushedEmpty;
		static Material *suspicion;
		static Material *bomb;

		static Vector<Texture *>overlays;

		void SetMaterial(CellMaterial cellMaterial);
		void SetOverlay(unsigned int idx);
	#pragma endregion
	};
}