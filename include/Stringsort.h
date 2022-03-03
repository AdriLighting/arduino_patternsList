/*
   AUTHOR
      https://github.com/leftCoast/LC_baseTools
         https://forum.arduino.cc/t/alphabetizing-string-array/694356/11

*/

#ifndef STRINGSORT_H
#define STRINGSORT_H

#include "lists/lists.h"
#include "resizeBuff/resizeBuff.h"

// The class setup for the list object.
class stringObj : public linkListObj {
   public:
                  stringObj(char* inStr);
   virtual        ~stringObj(void);

   virtual  bool   isGreaterThan(linkListObj* compObj);   // Are we greater than the obj being passed in? Primary sorting function.
   virtual  bool   isLessThan(linkListObj* compObj);      // Are we less than the obj being passed in? Primary sorting function.

            char* mStr;
};

// Class setup for our stringList
class stringList :   public linkList {

  public:
                  stringList(void);
   virtual        ~stringList(void);

                  void addString(String* inStr);
                  void addString(char* inStr);
                  void printStrings(void);
};


#endif // STRINGSORT_H