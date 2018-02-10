#include "Cells.h"
#include "Cell.h"
#include "CellComponent.h"
#include <Urho3D/Container/VectorBase.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Random.h>
#include <Urho3D/IO/Log.h>

namespace minesweeper
{
	Cells::Cells()
	{
		for (unsigned int row = 0; row < cellsRows; row++)
		{
			for (unsigned int col = 0; col < cellsRows; col++)
			{
				Cell *cell = new Cell(this, row, col);
				cells.Push(cell);
			}
		}

		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			cells[idx]->CollectNeighbours();
		}
	}

	Cells::~Cells()
	{
		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			Cell *cell = cells[idx];
			delete cell;
		}

		this->cells.Clear();
	}

	void Cells::PreGrowt()
	{
		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			Cell *cell = cells[idx];
			cell->PreGrowt();
		}
	}

	void Cells::Reset()
	{
		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			cells[idx]->Reset();
		}
	}

	void Cells::Start(int excudeRow, int excludeCol)
	{
		unsigned int minesGenerated = 0;
		while (minesGenerated < minesCount)
		{
			int newRow = Random((int)cellsRows);
			int newCol = Random((int)cellsCols);

			if ((newRow != excudeRow) && (newCol != excludeCol))
			{
				Cell *cell = GetCell(newRow, newCol);
				if (!cell->IsMine())
				{
					cell->SetMine(true);
					minesGenerated++;
				}
			}
		}

		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			cells[idx]->Init();
		}
	}

	bool Cells::CheckWin()
	{
		int countMinesSelected = 0;
		for (unsigned int idx = 0; idx < cells.Size(); idx++)
		{
			Cell *cell = cells[idx];
			bool isMine = cell->IsMine();
			bool isSuspected = cell->IsSuspected();

			// false positive
			if (isSuspected && !isMine)
			{
				return false;
			}

			// not marked yet
			if (isMine && !isSuspected)
			{
				return false;
			}

			if (isMine && isSuspected)
			{
				countMinesSelected++;
			}
		}
		return countMinesSelected == minesCount;
	}

	Cell *Cells::GetCell(unsigned int row, unsigned int col)
	{
		unsigned int idx = row * cellsCols + col;
		return (idx < cells.Size())
			? cells[idx]
			: nullptr;
	}

	Cell *Cells::GetCell(CellComponent *cellComponent)
	{
		return (cellComponent == nullptr)
			? nullptr
			: GetCell(cellComponent->GetRow(), cellComponent->GetCol());
	}
}


unsigned int minesweeper::Cells::cellsRows = 10;
unsigned int minesweeper::Cells::cellsCols = 10;
unsigned int minesweeper::Cells::minesCount = 10;