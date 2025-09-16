#include "FPGASettings.h"

UDataTable* UFPGASettings::GetGameValuesTable() const
{
	return !GameValues.IsNull() ? GameValues.LoadSynchronous() : nullptr;
}
