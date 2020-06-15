#ifndef SOLVER_NAMES_H
#define SOLVER_NAMES_H

struct SolverNames
{
  // params
  static const char*    LOAD_SCALE;
  static const char*    ACCEPT;
  static const char*    DISCARD;
  static const char*    DO_SWITCH;
  static const char*    STOP_ARCL;
  static const char*    DONE;
  static const char*    TEMPORARY;
  static const char*    STEP_SIZE;
  static const char*    STEP_SIZE_0;
  static const char*    ALLOW_TMP;
  static const char*    TERMINATE;
  static const char*    CHECK_BOUNDS;
  static const char*    DISSIPATION_FORCE;
  static const char*    N_CONTINUES;
  static const char*    ADAPT_HOW;
  static const char*    ADAPT_FROM;
  static const char*    CHANGE_COUNT;

  // actions
  static const char*    TO_ARCL;
  static const char*    TO_DISP;
  static const char*    CHECK_COMMIT;
  static const char*    ADAPT_STEP;
  static const char*    GET_DISS_FORCE;
  static const char*    GET_STEP_SIZE;
  static const char*    SET_STEP_SIZE;
  static const char*    CONTINUE;
  static const char*    BE_CAREFUL;
  static const char*    STOP_CAREFUL;
};


#endif 
