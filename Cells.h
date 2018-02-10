#pragma once
#include <Urho3D/Container/Vector.h>

namespace minesweeper
{
	using namespace Urho3D;

	class Cell;
	class CellComponent;

	// holder for all cells
	class Cells
	{
	public:
		Cells();
		~Cells();

	#pragma region gameplay parameters
	public:
		static unsigned int cellsRows;
		static unsigned int cellsCols;
		static unsigned int minesCount;

	#pragma endregion

	public:
		// call begore every game
		void Reset();

		// call on first user click in game - generate minefield, cell [excudeRow, excludeCol] will be safe
		void Start(int excudeRow, int excludeCol);
		
		void PreGrowt();

		// return true if user win (mark all bombs)
		bool CheckWin();

		Cell *GetCell(unsigned int row, unsigned int col);
		Cell *GetCell(CellComponent *cellComponent);

	private:
		Vector<Cell *>cells;
	};
}