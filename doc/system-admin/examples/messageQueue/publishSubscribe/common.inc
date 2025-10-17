// Common parts of the message queue examples

DATAPOOL
  STRUCT
    Wheather {
      STRING
        stationName { LABEL="Station name", LABEL }
      , timestamp   { LABEL="Timestamp", STRING_TIME }
      ;
      REAL
        temperature { LABEL="Temperature", UNIT="[°C]" }
      , airPressure { LABEL="Air pressure", UNIT="[hPa]" }
      ;
    }
  ;
  Wheather weather;
END DATAPOOL;

UI_MANAGER
  FIELDGROUP
    weather_fg (
      LABEL(weather.stationName) weather.stationName { COLSPAN=2 }
    , LABEL(weather.timestamp)   weather.timestamp   { COLSPAN=2 }
    , LABEL(weather.temperature) weather.temperature:6:1
        UNIT(weather.temperature)
    , LABEL(weather.airPressure) weather.airPressure*1e-2:6:1
        UNIT(weather.airPressure)
    )
  ;

  // Hide STD_WINDOW and LOG_WINDOW from MAIN form
  FORM
    stdwin_form { "Standard output", HIDE_CYCLE
                , CLOSE_BUTTON=NONE } (
      STD_WINDOW { SIZE=24*80 }
    )
  , logwin_form { "Log output", HIDE_CYCLE
                , CLOSE_BUTTON=NONE } (
      LOG_WINDOW { SIZE=24*80 }
    )
  ;
END UI_MANAGER;

STREAMER
  weather_stream { JSON } (
    weather
  );
END STREAMER;
