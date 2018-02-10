#pragma once

#include "Sample.h"

namespace minesweeper
{
	class CellComponent;
	class Cells;
}
using namespace minesweeper;

/// This first example, maintaining tradition, prints a "Hello World" message.
/// Furthermore it shows:
///     - Using the Sample / Application classes, which initialize the Urho3D engine and run the main loop
///     - Adding a Text element to the graphical user interface
///     - Subscribing to and handling of update events
class Minesweeper : public Sample
{
    URHO3D_OBJECT(Minesweeper, Sample);

public:
    /// Construct.
    explicit Minesweeper(Context* context);

#pragma region init and stop
public:
    /// Setup after engine initialization and before running the main loop.
    void Start() override;

private:
	void LoadResources();
	void CreateScene();
	void SubscribeToEvents();

public:
	void Stop() override;
#pragma endregion



#pragma region Event System
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
	void HandleTouchBegin(StringHash eventType, VariantMap& eventData);
	void HandleTouchEnd(StringHash eventType, VariantMap& eventData);
#pragma endregion

	/// Get cell by mouse click
	CellComponent *Raycast(int x, int y);

#pragma region UI for win/loose window
	bool isUIVisible;
	SharedPtr<Window> window_;
	SharedPtr<UIElement> uiRoot_;
	void ShowWin();
	void ShowLoose();
	void ShowReplayWindow(String text);
	
	void HandleReplayPressed(StringHash eventType, VariantMap& eventData);
#pragma endregion

#pragma region touch input
private:
	unsigned int touchStartTime;
#pragma endregion

#pragma region user action
private:
	void TryActivate(int x, int y);
	void TrySuspect(int x, int y);
#pragma endregion 


	Cells *cells;

	// we generate mine field on user first click to aviod loose on game start
	bool isFirstClick;

protected:
	/// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
	String GetScreenJoystickPatchString() const override {
		return
			"<patch>"
			"    <add sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]\">"
			"        <attribute name=\"Is Visible\" value=\"false\" />"
			"    </add>"
			"</patch>";
	}

};
