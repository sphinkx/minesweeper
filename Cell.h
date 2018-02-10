#pragma once
#include <Urho3D/Container/Vector.h>

namespace minesweeper
{
	using namespace Urho3D;

	class Cells;
	class CellComponent;

	enum ActivationResult
	{
		// user click on safe cell
		Clear,
		// user click on mine
		Mine
	};

	// Represent logic for one cell. For UI see CellComponent
	class Cell
	{
	public:
		Cell(Cells *parent_, unsigned int row_, unsigned int col_);
		~Cell();

	private:
		Cells *parent;
		unsigned int row;
		unsigned int col;

	#pragma region link with CellComponent
	private:
		CellComponent *component;
	public:
		void SetComponent(CellComponent  *component_) { component = component_; }
		CellComponent *GetComponent() { return component; }
	#pragma endregion
	

	public:
		// one-time initialization
		void Init();
		// call before every game
		void Reset();
		
		// user click on cell
		ActivationResult Activate();

	#pragma region link with neighbours cells (8-way link)
	public:
		// find all neighbours cells and store in `neighbours` 
		void CollectNeighbours();
	private:
		Vector<Cell *>neighbours;

		int countMineNeighours;
	#pragma endregion

	#pragma region growt
	private:
		bool growt;
	public:
		void PreGrowt();

		// try auto-push cell (when neighbour cell pushed by user)
		void TryGrowt();
	#pragma endregion

	#pragma region Flags
	private:
		/// true if mine placed in this cell
		bool mine;
	public:
		inline void SetMine(bool mine_) { this->mine = mine_; }
		inline bool IsMine() { return mine; }

	private:
		/// true, if cell activated (i.e. user already push this cell)
		bool activated;
	public:
		inline bool IsActivated() { return activated; }

	private:
		/// true, if user mark this cell as  suspected
		bool suspected;
	public:
		void SetSuspected(bool suspected_);
		inline bool IsSuspected() { return suspected; }
	#pragma endregion 
	};
}