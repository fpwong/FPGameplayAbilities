// Fill out your copyright notice in the Description page of Project Settings.

#include "FPGAHelperLibrary.h"
//#include "steam/steam_api.h"
//#include "steam/steamclientpublic.h"
#include "Perception/AISense.h"
//
//void UFPGAHelperLibrary::GetSteamFriends(TArray<FSteamFriends>& OutFriends, int32& OutFriendCount)
//{
//	// create a temp steam friends struct
//	FSteamFriends TempSteamFriends;
//
//	// get the total count of how many steam friends we have. 
//	int SteamFriendsCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
//
//	// set our OutFriendCount variable to be equal to the SteamFriendsCount we just got above
//	OutFriendCount = SteamFriendsCount;
//
//	// check to make sure the user is logged in 
//	if (SteamFriendsCount == -1)
//	{
//		// user not logged in 
//		return;
//	}
//
//	// loop through and find all friends data
//	for (int FriendIndex = 0; FriendIndex < SteamFriendsCount; FriendIndex++)
//	{
//		// variables for the friends avatar data
//		int AvatarSizeIndex = 0;
//		uint32 AvatarWidth = 0;
//		uint32 AvatarHeight = 0;
//
//		// get the current friends CSteamID
//		CSteamID FriendsSteamID = SteamFriends()->GetFriendByIndex(FriendIndex, k_EFriendFlagImmediate);
//
//		// get the friends medium avatar index handle
//		AvatarSizeIndex = SteamFriends()->GetMediumFriendAvatar(FriendsSteamID);
//
//		// get the actual image size so we can reserve the correct memory 
//		bool bGetImageSize = SteamUtils()->GetImageSize(AvatarSizeIndex, &AvatarWidth, &AvatarHeight);
//		if (!bGetImageSize)
//		{
//			// unable to get image size
//		}
//
//		// create buffer for the RGBA data (* 4 because R(1) G(2) B(3) A(4) )
//		uint8* AvatarRGBA = new uint8[AvatarHeight * AvatarWidth * 4];
//
//		bool GetImageRGBA = SteamUtils()->GetImageRGBA(AvatarSizeIndex, AvatarRGBA, AvatarHeight * AvatarWidth * 4 * sizeof(char));
//		if (!GetImageRGBA)
//		{
//			// error getting the RGBA data
//		}
//
//		// create a placeholder texture to fill the buffer in
//		UTexture2D* AvatarTexture = UTexture2D::CreateTransient(AvatarWidth, AvatarHeight, PF_R8G8B8A8);
//
//		// lock the BulkData and MemCopy
//		uint8* MipData = (uint8*) AvatarTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
//		FMemory::Memcpy(MipData, (void*) AvatarRGBA, AvatarHeight * AvatarWidth * 4);
//		AvatarTexture->PlatformData->Mips[0].BulkData.Unlock();
//
//		//clear up the memory
//		delete[] AvatarRGBA;
//
//		// update the texture2d settings
//		AvatarTexture->PlatformData->NumSlices = 1;
//		AvatarTexture->NeverStream = true;
//		AvatarTexture->UpdateResource();
//
//		// set the TempSteamFriends Struct data
//
//		// get the friends display name store it in the temporary struct
//		TempSteamFriends.DisplayName = SteamFriends()->GetFriendPersonaName(FriendsSteamID);
//
//		// get the friends player avatar store it in the temporary struct
//		TempSteamFriends.PlayerAvatar = AvatarTexture;
//
//		// add the temporary struct data to the OutFriends array	
//		OutFriends.Add(TempSteamFriends);
//	}
//}

bool UFPGAHelperLibrary::CheckAffiliation(FAISenseAffiliationFilter Affiliation, AActor* ActorA, AActor* ActorB)
{
	if (ActorA != nullptr && ActorB != nullptr)
	{
		if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(ActorA))
		{
			return FAISenseAffiliationFilter::ShouldSenseTeam(TeamAgent, *ActorB, Affiliation.GetAsFlags());
		}
	}

	return false;
}
