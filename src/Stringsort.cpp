/*
   AUTHOR
      https://github.com/leftCoast/LC_baseTools
         https://forum.arduino.cc/t/alphabetizing-string-array/694356/11

*/

#include "../include/Stringsort.h"
#include "../include/program.h"


//**************************************************************
//  Setting up a linked list of strings that can be sorted.. 
//  First is the list object that holds the string info. and
//  can compare itself to another string object..
//**************************************************************

String myString;     // These two are for doing the compare. They are globals.
String theirString;


// Now the code we add to the class..
stringObj::stringObj(char* inStr)
   : linkListObj() {

   mStr = NULL;                              // Defualt our dynamic memory pointer to NULL
   if (resizeBuff(strlen(inStr)+1,&mStr)) {  // IF we were able to allocate enough RAM for holding our string..
      strcpy(mStr,inStr);                    // Stuff a copy of what we got into our object.
   }
}


// Recycle the RAM we used.
stringObj::~stringObj(void)  { resizeBuff(0,&mStr); }
 

// Are we greater than the obj being passed in? Primary sorting function.
bool stringObj::isGreaterThan(linkListObj* compObj) {
      
      stringObj* them;
      
      them = (stringObj*)compObj;      // We get a pointer to a linkListObj. We know its a stringObj. Just say it is.
      myString = mStr;                 // Load up the gloabl String, myString with our string.
      theirString = them->mStr;        // Load up the gloabl String, theirString with the incoming object's string.
      return myString>theirString;     // Return the result of the compare.
}


// Are we less than the obj being passed in? Primary sorting function.
bool stringObj::isLessThan(linkListObj* compObj) {      

   stringObj* them;
   
   them = (stringObj*)compObj;      // We get a pointer to a linkListObj. We know its a stringObj. Just say it is.
   myString = mStr;                 // Load up the gloabl String, myString with our string.
   theirString = them->mStr;        // Load up the gloabl String, theirString with the incoming object's string.
   return myString<theirString;     // Return the result of the compare.
}


//**************************************************************
// That's all for the string list object. Now we code up the 
// changes that make our stringList..
//**************************************************************


// Constructor, nothing to do..
stringList::stringList(void)
   : linkList() {  }


// Destructor, nothing to do..  
stringList::~stringList(void) {  }


// Add in a String object. We use a pointer and call the c string addString() method with that.
void stringList::addString(String* inStr) {
   char buffer[255]; 
   sprintf(buffer, "%s", inStr->c_str()); 
   addString(buffer); 
}

// Here's where we actually add the string to the list.
void stringList::addString(char* inStr) {

   stringObj*  newItem;

   newItem = new stringObj(inStr);  // Create the new list node with the incoming c string.
   addToTop(newItem);               // Stick it in the list, anywhere will do. Top is fastest.
}


// Sort and print all our strings to the serial monitor.
void stringList::printStrings(void) {

   stringObj* trace;
   
   sort(true);
   trace = (stringObj*)getFirst();
   while(trace) {
      ProgramPtrGet()->item_add(trace->mStr);
      // Serial.println(trace->mStr);
      trace = (stringObj*)trace->getNext();
   }
}


// ***********************************************
// The main program..
// ***********************************************


