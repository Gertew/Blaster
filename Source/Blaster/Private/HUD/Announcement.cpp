// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Announcement.h"
#include "Components/TextBlock.h"

void UAnnouncement::SetAnnouncementCountdown(float CountdownTime)
{
	if (WarmupTimeText)
	{
		if (CountdownTime < 0.0f)
		{
			WarmupTimeText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FText TextCountdown = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
		WarmupTimeText->SetText(TextCountdown);
	}
}

void UAnnouncement::SetAnnouncement(FText Announcement)
{
	if (AnnouncementText)
	{
		AnnouncementText->SetText(Announcement);
	}
}

void UAnnouncement::SetInfo(FText Info)
{
	if (InfoText)
	{
		InfoText->SetText(Info);
	}
}
