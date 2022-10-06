# TO DO

* Generalize menu treatment so we can have a tree
  -  Main menu - "Edit notes", "Dump notes", "Restore notes"
  -  Callbacks/labels for Green/Blue knobs -- because dunno what they're doing.
  -  Set "uptree" context, except for "root" (main menu).
* Pull LED handling into an observer pattern for "chord" processing.
  -  Only the "current" note will change with adjustment
  -  Of course, if there's no chord, that could still be on all 3 LEDs.
  -  It really means we need an "observer" array.
  -  And maybe the on-board vs on-piano LEDs get treated differently?
  -  One of the knobs can "rotate" the chord?
