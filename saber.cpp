#include "sabermodel.h"

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSaberModel(int x, int y, int w, int h, char *label)
{
	return new SaberModel(x, y, w, h, label);
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SaberModel
void SaberModel::draw()
{
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ChooseCostume(VAL(COSTUME));

	LocateBody(VAL(XPOS), VAL(YPOS), VAL(ZPOS), VAL(ROTATE));
	setExcaliburTransparency(VAL(SWORD_TRANSPARENCY));
	RotateExcalibur(0.0, 90.0, LOWER_ARM_SLASH*VAL(SLASH)<40.0 ? LOWER_ARM_SLASH*VAL(SLASH) : 40.0, "yzx");
	RotateRightUpperArm(1.5*VAL(SLASH), 0.0, 0.0);
	RotateRightLowerArm(LOWER_ARM_SLASH*VAL(SLASH), 0.0, LOWER_ARM_SLASH*VAL(SLASH)<40.0 ? -LOWER_ARM_SLASH*VAL(SLASH) : -40.0, "zxy");
	RotateLeftUpperArm(1.5*VAL(SLASH), 0.0, 0.0);
	RotateLeftLowerArm(LOWER_ARM_SLASH*VAL(SLASH), 0.0, LOWER_ARM_SLASH*VAL(SLASH)<40.0 ? LOWER_ARM_SLASH*VAL(SLASH) : 40.0, "zxy");

	treeRoot->Render();
	
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[SWORD_TRANSPARENCY] = ModelerControl("SwordTransparency", 0.0, 1.0, 0.1f, 1);
	controls[ROTATE] = ModelerControl("Rotate", -180, 180, 1, 0);
	controls[SLASH] = ModelerControl("Slash", 0, 90, 1, 0);
	controls[COSTUME] = ModelerControl("Costume", 1, 3, 1, 1);

	ModelerApplication::Instance()->Init(&createSaberModel, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}
