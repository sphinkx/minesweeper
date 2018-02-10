#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/Core/Timer.h>

#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>

#include "Minesweeper.h"
#include "Cell.h"
#include "Cells.h"
#include "CellComponent.h"

#include <Urho3D/DebugNew.h>
using namespace minesweeper;

// Expands to this example's entry-point
URHO3D_DEFINE_APPLICATION_MAIN(Minesweeper)

Minesweeper::Minesweeper(Context* context) :
    Sample(context),
	uiRoot_(GetSubsystem<UI>()->GetRoot()),
	isUIVisible(false),
	isFirstClick(true)
{
	context->RegisterFactory<CellComponent>();
}

void Minesweeper::Start()
{
    // Execute base class startup
    Sample::Start();

	cells = new Cells();

	LoadResources();
	CreateScene();
    SubscribeToEvents();

	cells->Reset();

#pragma region test touch system
	Input *input = GetSubsystem<Input>();
	input->SetTouchEmulation(false);
#pragma endregion

	// show cursor
    Sample::InitMouseMode(MM_FREE);
}

void Minesweeper::LoadResources()
{
	ResourceCache* resourceCache = GetSubsystem<ResourceCache>();

	CellComponent::ordinal = resourceCache->GetResource<Material>("minesweeper/Ordinal.xml");
	CellComponent::pushed = resourceCache->GetResource<Material>("minesweeper/Pushed.xml");
	CellComponent::pushedEmpty = resourceCache->GetResource<Material>("minesweeper/PushedEmpty.xml");
	CellComponent::suspicion = resourceCache->GetResource<Material>("minesweeper/Suspicion.xml");
	CellComponent::bomb = resourceCache->GetResource<Material>("minesweeper/Bomb.xml");

	for (unsigned int i = 0; i <= 8;i++)
	{
		String path = "";
		path.AppendWithFormat("minesweeper/0%d.png", i);
		Texture2D *texture2D = resourceCache->GetResource<Texture2D>(path);
		CellComponent::overlays.Push(texture2D);
	}
}


void Minesweeper::CreateScene()
{
	ResourceCache* resourceCache = GetSubsystem<ResourceCache>();

	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();

#pragma region cells
	for (unsigned int row = 0; row < Cells::cellsRows; row++)
	{
		for (unsigned int col = 0; col < Cells::cellsRows; col++)
		{
			Node* node = scene_->CreateChild("Cell");

			StaticModel* staticModel = node->CreateComponent<StaticModel>();
			staticModel->SetModel(resourceCache->GetResource<Model>("minesweeper/Cell.mdl"));
			staticModel->SetMaterial(CellComponent::ordinal);

			CellComponent *cellComponent = node->CreateComponent<CellComponent>();
			cellComponent->SetCoord(row, col);
			cellComponent->SetMaterial(CellMaterial::Ordinal);

			cells->GetCell(row, col)->SetComponent(cellComponent);
		}
	}
#pragma endregion


#pragma region light
	{
		Node* lightNode = scene_->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
		auto* light = lightNode->CreateComponent<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
	}
#pragma endregion 

#pragma region camera
	{
		cameraNode_ = scene_->CreateChild("Camera");
		Camera *camera = cameraNode_->CreateComponent<Camera>();
		camera->SetOrthographic(true);
		camera->SetOrthoSize(10.0f);

		cameraNode_->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
		cameraNode_->SetRotation(Quaternion(90.0f, 0.0f, 0.0f));
	}
#pragma endregion

#pragma region UI
	{
		auto* style = resourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");
		uiRoot_->SetDefaultStyle(style);
	}
#pragma endregion UI

#pragma region viewport
	{
		Renderer* renderer = GetSubsystem<Renderer>();

		SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
		renderer->SetViewport(0, viewport);
	}
#pragma endregion
}


void Minesweeper::SubscribeToEvents()
{
	// Subscribe HandleUpdate() function for processing update events
	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Minesweeper, HandleUpdate));

	SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(Minesweeper, HandleTouchBegin));
	SubscribeToEvent(E_TOUCHEND, URHO3D_HANDLER(Minesweeper, HandleTouchEnd));

	//if (GetPlatform() == "Android" || GetPlatform() == "iOS")
	{
		// add touch input for mibile platforms

	}
}


void Minesweeper::Stop()
{
	delete this->cells;
	CellComponent::overlays.Clear();
	Sample::Stop();
}



void Minesweeper::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
	Input* input = GetSubsystem<Input>();

	if (input->GetMouseButtonPress(MOUSEB_LEFT))
	{
		auto* ui = GetSubsystem<UI>();
		IntVector2 pos = ui->GetCursorPosition();
		TryActivate(pos.x_, pos.y_);
	}

	if (input->GetMouseButtonPress(MOUSEB_RIGHT))
	{
		auto* ui = GetSubsystem<UI>();
		IntVector2 pos = ui->GetCursorPosition();
		TrySuspect(pos.x_, pos.y_);
	}
}

void Minesweeper::HandleTouchBegin(StringHash eventType, VariantMap& eventData)
{
	using namespace TouchBegin;
	int touchId = eventData[P_TOUCHID].GetInt();
	if (touchId == 0)
	{
		touchStartTime = Time::GetSystemTime();
	}
}

void Minesweeper::HandleTouchEnd(StringHash eventType, VariantMap& eventData)
{
	using namespace TouchEnd;

	int touchId = eventData[P_TOUCHID].GetInt();
	if (touchId == 0)
	{
		int x = eventData[P_X].GetInt();
		int y = eventData[P_Y].GetInt();

		unsigned int touchEndTime = Time::GetSystemTime();
		if (touchEndTime - touchStartTime > 300)
		{
			TrySuspect(x, y);
		}
		else
		{
			TryActivate(x, y);
		}
	}
}


void Minesweeper::TryActivate(int x, int y)
{
	CellComponent *cellComponent = Raycast(x, y);
	if (cellComponent)
	{
		// We dont let user loose on first click. Generate minefiled by first click, excluding selected cell.
		if (isFirstClick)
		{
			isFirstClick = false;
			cells->Start(cellComponent->GetRow(), cellComponent->GetCol());
		}
		Cell *cell = cells->GetCell(cellComponent);
		if (cell->Activate() == ActivationResult::Mine)
		{
			ShowLoose();
		}
	}
}

void Minesweeper::TrySuspect(int x, int y)
{
	CellComponent *cellComponent = Raycast(x, y);
	if (cellComponent)
	{
		Cell *cell = cells->GetCell(cellComponent);
		cell->SetSuspected(!cell->IsSuspected());

		if (cells->CheckWin())
		{
			ShowWin();
		}
	}
}

void Minesweeper::ShowWin()
{
	ShowReplayWindow("Win!");
}

void Minesweeper::ShowLoose()
{
	ShowReplayWindow("Loose!");
}


void Minesweeper::ShowReplayWindow(String text)
{
	isUIVisible = true;

	ResourceCache* resourceCache = GetSubsystem<ResourceCache>();

	window_ = new Window(context_);
	uiRoot_->AddChild(window_);
	
	// Create window
	window_->SetMinWidth(384);
	window_->SetMinHeight(128);
	window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
	window_->SetAlignment(HA_CENTER, VA_CENTER);
	window_->SetName("Window");

	// Create "titlebar" and text
	auto* titleBar = new UIElement(context_);
	titleBar->SetMinSize(0, 24);
	titleBar->SetVerticalAlignment(VA_TOP);
	titleBar->SetLayoutMode(LM_HORIZONTAL);

	auto* windowTitle = new Text(context_);
	windowTitle->SetName("WindowTitle");
	windowTitle->SetText(text);

	// Create "replay" button
	auto* buttonReplay = new Button(context_);
	buttonReplay->SetName("CloseButton");

	Text* t = new Text(context_);
	t->SetFont(resourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 20);
	t->SetHorizontalAlignment(HA_CENTER);
	t->SetVerticalAlignment(VA_CENTER);
	t->SetName("Replay");
	t->SetText("Replay");
	t->SetStyle("Text");
	buttonReplay->AddChild(t);


	// Add the controls to the title bar
	titleBar->AddChild(windowTitle);
	//titleBar->AddChild(buttonReplay);

	// Add the title bar to the Window
	window_->AddChild(titleBar);
	window_->AddChild(buttonReplay);

	// Apply styles
	window_->SetStyleAuto();
	windowTitle->SetStyleAuto();
	buttonReplay->SetStyleAuto();

	SubscribeToEvent(buttonReplay, E_RELEASED, URHO3D_HANDLER(Minesweeper, HandleReplayPressed));
}


void Minesweeper::HandleReplayPressed(StringHash eventType, VariantMap& eventData)
{
	uiRoot_->RemoveChild(window_);
	isUIVisible = false;
	isFirstClick = true;

	cells->Reset();
}


CellComponent *Minesweeper::Raycast(int x, int y)
{
	// do nothing when pop-up visible
	if (isUIVisible)
	{
		return nullptr;
	}

	const float maxDistance = 250.0f;
	Vector3 hitPos;
	Drawable* hitDrawable = nullptr;

	auto* ui = GetSubsystem<UI>();
	if (ui)
	{
		if (ui->GetCursor() && !ui->GetCursor()->IsVisible())
		{
			return nullptr;
		}

		if (ui->GetElementAt(IntVector2(x, y), true))
		{
			return nullptr;
		}
	}

	Graphics* graphics = GetSubsystem<Graphics>();
	Camera* camera = cameraNode_->GetComponent<Camera>();
	Ray cameraRay = camera->GetScreenRay((float)x / graphics->GetWidth(), (float)y / graphics->GetHeight());
	PODVector<RayQueryResult> results;
	RayOctreeQuery query(results, cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
	scene_->GetComponent<Octree>()->RaycastSingle(query);
	if (results.Size())
	{
		RayQueryResult& result = results[0];
		hitPos = result.position_;
		hitDrawable = result.drawable_;

		Node* node = hitDrawable->GetNode();
		if (node)
		{
			return node->GetComponent<CellComponent>();
		}
	}

	return nullptr;
}


