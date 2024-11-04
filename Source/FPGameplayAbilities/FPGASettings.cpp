#include "FPGASettings.h"

UDataTable* UFPGASettings::GetGameValuesTable()
{
	return !GameValues.IsNull() ? GameValues.LoadSynchronous() : nullptr;
}
