// This code originates from Dynamic Timescale by Partyzan
// All credit goes to him.
// https://reforger.armaplatform.com/workshop/62191221E50A878A-DynamicTimescale

modded class SCR_TimeAndWeatherHandlerComponent : SCR_BaseGameModeComponent
{
	[Attribute("1", UIWidgets.Slider, desc: "Dusk and Dawn Timescale", category: "DuskDawnTimescale", params: "1 64 1")]
	protected int m_iDuskDawnTimescale;
	[Attribute("4", UIWidgets.Slider, desc: "Day Timescale", category: "DayTimescale", params: "1 64 1")]
	protected int m_iDayTimescale;
	[Attribute("32", UIWidgets.Slider, desc: "Night Timescale", category: "NightTimescale", params: "1 64 1")]
	protected int m_iNightTimescale;
	
	float DuskDawnTimescale()
	{
		return m_iDuskDawnTimescale;
	}
	float DayTimescale()
	{
		return m_iDayTimescale;
	}
	float NightTimescale()
	{
		return m_iNightTimescale;
	}
	
	override void SetupDaytimeAndWeather(int hours, int minutes, int seconds = 0, string loadedWeatherState = "", bool loadDone = false)
	{
		if (m_bSavedSettingApplied)
			return;

		m_bSavedSettingApplied = loadDone;

		ChimeraWorld world = ChimeraWorld.CastFrom(GetOwner().GetWorld());
		if (!world)
			return;
		
		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;

		float sunrise, morning, evening, sunset;

		// Use world-calculated sunrise and sunset values if possible, otherwise use defaults
		if (manager.GetSunriseHour(sunrise))
		{
			manager.GetSunsetHour(sunset);
		}
		else
		{
			sunrise = DEFAULT_DAYTIME_START;
			sunset = DEFAULT_DAYTIME_END;
		}

		if (m_bRandomStartingDaytime && !loadDone)
		{
			// Compile a list of presets based on the sunrise and sunset times of current world if we're randomizing
			morning = sunrise + 0.25;	// Just so it's not still completely dark at the start
			float noon = (sunrise + sunset) * 0.5;
			float afternoon = (noon + sunset) * 0.5;
			evening = sunset - 0.5;
			float night = noon + 12;

			if (night >= 24)
				night -= 24;

			array<float> startingTimes = {morning, noon, afternoon, evening, night};

			// Add weights so evening / night is a bit more rare
			Math.Randomize(-1);
			int index = SCR_ArrayHelper.GetWeightedIndex({25, 20, 20, 25, 10}, Math.RandomFloat01());
			float startingTime;

			if (startingTimes.IsIndexValid(index))
				startingTime = startingTimes[index];
			else
				startingTime = startingTimes.GetRandomElement();

			manager.TimeToHoursMinutesSeconds(startingTime, hours, minutes, seconds);
			if (m_bRandomStartingWeather && !loadDone)
			{
				array<ref WeatherState> weatherStates = {};
				manager.GetWeatherStatesList(weatherStates);
	
				if (!weatherStates.IsEmpty())
				{	
					//Print("------------------------------weterstate");
					//Print(weatherStates[0].GetStateName());
					//Print(startingTime);
					if ((startingTime > sunset - 1) || (startingTime < sunrise - 0.5)) //(m_bRandomStartingDaytime > manager.GetSunsetHour(sunset))
					{
						array<ref WeatherState> nightweathers = {weatherStates[0],weatherStates[1]};
						WeatherState nightweather;
						nightweather = nightweathers.GetRandomElement();
						Print("------------------------------sunsetstate");
						Print(nightweather.GetStateName());
						Math.Randomize(-1);
						manager.ForceWeatherTo(false, nightweather.GetStateName());
					}
					else
					{
						Math.Randomize(-1);
						manager.ForceWeatherTo(false, weatherStates.GetRandomElement().GetStateName());
					}
					
					//Math.Randomize(-1);
					//manager.ForceWeatherTo(false, weatherStates.GetRandomElement().GetStateName());
				}
			}
		}

		
		//if (!loadedWeatherState.IsEmpty())
			//manager.ForceWeatherTo(false, loadedWeatherState);
		
		manager.SetHoursMinutesSeconds(hours, minutes, seconds);

		// Periodically check if the acceleration is correct, based on time of day
		// SetTimeEvent is not usable since it requires changing Periodicity attribute directly in the manager entity in the world layer
		if (m_fDayTimeAcceleration != 1 || m_fNightTimeAcceleration != 1)
		{
			HandleDaytimeAcceleration(true);
			GetGame().GetCallqueue().Remove(HandleDaytimeAcceleration);
			GetGame().GetCallqueue().CallLater(HandleDaytimeAcceleration, DAYTIME_CHECK_PERIOD, true, false);
		}
	}
	override protected void HandleDaytimeAcceleration(bool setup = false)
	{
		//Print("------------------------------etotest");
		ChimeraWorld world = ChimeraWorld.CastFrom(GetOwner().GetWorld());
		
		if (!world)
			return;
		
		TimeAndWeatherManagerEntity manager = world.GetTimeAndWeatherManager();
		if (!manager)
			return;
		
		float sunrise, sunset;
		int hour, minute, sec;
		manager.GetHoursMinutesSeconds(hour, minute, sec);
		// Use world-calculated sunrise and sunset values if possible, otherwise use defaults
		if (manager.GetSunriseHour(sunrise))
		{
			manager.GetSunsetHour(sunset);
		}
		else
		{
			sunrise = DEFAULT_DAYTIME_START;
			sunset = DEFAULT_DAYTIME_END;
		}
		
		int dayrange = DayTimescale();
		int nightrange = NightTimescale();
		int dawnrange = DuskDawnTimescale();
		if ((hour < (sunrise - 0.25)) || (hour > (sunset + 0.25)))
		{
			if (m_bDaytimeAcceleration || setup)
			{
				m_bDaytimeAcceleration = false;
				
				manager.SetDayDuration(DAY_DURATION / nightrange);
			}
		}
		else
		{
			m_bDaytimeAcceleration = true;
			//if (!m_bDaytimeAcceleration || setup)
			if ((hour > (sunrise + 1)) && (hour < (sunset - 1)))
			{
				manager.SetDayDuration(DAY_DURATION / dayrange);
			}
			else
				manager.SetDayDuration(DAY_DURATION / dawnrange);
		}
	}
}