
#ifndef REDISPLAYLISTENER_H
#define REDISPLAYLISTENER_H
/** RedisplayListener
    Die Klasse RedisplayListener ist eine abstrakte Interface-Klasse
    zur Anzeige von Grafikdaten auf X11
*/

class Gdev;

class RedisplayListener {
public:
  /**@name methods */
  //@{
  /** RedisplayListener-Interface.
      schreibt die Grafik auf das Device
      @param g Grafikdevice
  */
  virtual void repaint( Gdev &g, bool allPages = false )=0;
  virtual void refresh( Gdev &g )=0;
  //@}
};
#endif
