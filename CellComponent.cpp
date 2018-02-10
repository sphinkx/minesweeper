#include "CellComponent.h"
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Node.h>
using namespace Urho3D;

namespace minesweeper
{
	CellComponent::CellComponent(Context* context)
		:LogicComponent(context)
	{
	}

	void CellComponent::SetCoord(unsigned int row_, unsigned int col_)
	{
		row = row_;
		col = col_;

		const float scale = 1.0f;
		Node *node = this->GetNode();
		node->SetPosition(Vector3(-5.0f + col * scale, 0.0f, -4.5f + row * scale));
		node->SetScale(scale);
		node->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
	}

	void CellComponent::SetMaterial(CellMaterial cellMaterial)
	{
		StaticModel *staticModel = this->GetNode()->GetComponent<StaticModel>();

		switch (cellMaterial)
		{
		case CellMaterial::Ordinal:
			staticModel->SetMaterial(ordinal);
			break;

		case CellMaterial::Pushed:
			staticModel->SetMaterial(pushed);
			break;

		case CellMaterial::Suspicion:
			staticModel->SetMaterial(suspicion);
			break;

		case CellMaterial::Bomb:
			staticModel->SetMaterial(bomb);
			break;
		}
	}

	void CellComponent::SetOverlay(unsigned int countMineNeighours)
	{
		Node *node = this->GetNode();
		StaticModel *staticModel = node->GetComponent<StaticModel>();
		staticModel->SetMaterial(pushed->Clone());
		Material *material = staticModel->GetMaterial();
		material->SetTexture(TextureUnit::TU_DIFFUSE, overlays[countMineNeighours]);
	}

}

Material * minesweeper::CellComponent::ordinal = nullptr;
Material * minesweeper::CellComponent::pushed = nullptr;
Material * minesweeper::CellComponent::pushedEmpty = nullptr;
Material * minesweeper::CellComponent::suspicion = nullptr;
Material * minesweeper::CellComponent::bomb = nullptr;
Vector<Texture *>minesweeper::CellComponent::overlays;