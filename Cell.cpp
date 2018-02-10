#include "Cell.h"
#include "Cells.h"
#include "CellComponent.h"
#include <Urho3D/Container/VectorBase.h>
#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Random.h>



namespace minesweeper
{
	Cell::Cell(Cells *parent_, unsigned int row_, unsigned int col_)
		:parent(parent_), 
		row(row_), col(col_),
		component(nullptr)

	{
		Reset();
	}


	Cell::~Cell()
	{

	}

	void Cell::Init()
	{
		countMineNeighours = 0;

		for (unsigned i = 0; i < neighbours.Size(); ++i)
		{
			Cell *neighbour = neighbours[i];
			if (neighbour->IsMine())
			{
				countMineNeighours++;
			}
		}
	}


	void Cell::Reset()
	{
		mine = false;
		activated = false;
		suspected = false;
		countMineNeighours = 0;
		if (component)
		{
			component->SetMaterial(CellMaterial::Ordinal);
		}
	}



	ActivationResult Cell::Activate()
	{
		// click on activated cell
		if (this->activated)
		{
			return ActivationResult::Clear;
		}

		// missclick from user - LMB on cell, marked as `mine`
		if (this->suspected)
		{
			return ActivationResult::Clear;
		}

		// click on mine cell
		if (this->mine)
		{
			this->component->SetMaterial(CellMaterial::Bomb);
			return ActivationResult::Mine;
		}

		// click on clear cell - try activate all neighbours cells
		parent->PreGrowt();
		TryGrowt();
		return ActivationResult::Clear;
	}



	void Cell::CollectNeighbours()
	{
		int intRow = (int)row;
		int intCol = (int)col;

		for (int neighbourRow = intRow - 1; neighbourRow <= intRow + 1; neighbourRow++)
		{
			for (int neighbourCol = intCol - 1; neighbourCol <= intCol + 1; neighbourCol++)
			{
				if ((neighbourRow == row) && (neighbourCol == col))
				{
					continue;
				}

				if ((neighbourRow < 0) || (neighbourRow >= (int)Cells::cellsRows))
				{
					continue;
				}

				if ((neighbourCol < 0) || (neighbourCol >= (int)Cells::cellsCols))
				{
					continue;
				}

				this->neighbours.Push(parent->GetCell(neighbourRow, neighbourCol));
			}
		}
	}


	void Cell::PreGrowt() 
	{ 
		growt = false; 
	}


	void Cell::TryGrowt()
	{
		if (growt || activated || suspected)
		{
			return;
		}
		growt = true;

		component->SetOverlay(countMineNeighours);
		activated = true;

		if (countMineNeighours == 0)
		{
			for (unsigned int idx = 0; idx < neighbours.Size(); idx++)
			{
				Cell *neighbour = neighbours[idx];
				neighbour->TryGrowt();
			}
		}
	}

	void Cell::SetSuspected(bool suspected_) 
	{ 
		// user missclick - RMB on already active cell
		if (activated)
		{
			return;
		}

		this->suspected = suspected_; 
		component->SetMaterial(suspected ? CellMaterial::Suspicion : CellMaterial::Ordinal);
	}
}
