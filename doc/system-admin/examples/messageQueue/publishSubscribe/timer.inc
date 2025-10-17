// Timer parts for publisher.des

DATAPOOL
  REAL t;
  STRING { EDITABLE }
    start { LABEL="Start", BUTTON, FUNC=start_timer_func }
  , stop  { LABEL="Stop" , BUTTON, FUNC=stop_timer_func }
  ;
END DATAPOOL;

UI_MANAGER
  FIELDGROUP
    timer_button_fg (
      start   stop
    )
  ;
END UI_MANAGER;

OPERATOR
  TIMER
    publish_weather_timer {
      FUNC=update_and_publish_weather_func
    }
  ;
END OPERATOR;

FUNCTIONS
  FUNC
    update_and_publish_weather_func {
      // update (simulate) weather data
      t += 0.1;
      weather.timestamp   = CURRENT_TIME;
      weather.temperature = 15 + SIN(t) * 10;
      weather.airPressure = 102300 + COS(t) * 3000;

      RUN ( publish_weather_func );
    }

  , start_timer_func {
      START (
        publish_weather_timer
      , PERIOD=1
      , DELAY=5
      );
    }

  , stop_timer_func {
      STOP ( publish_weather_timer );
    }
  ;
END FUNCTIONS;
