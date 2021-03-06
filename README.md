# Foxit PDF Dictionary Plugin
Quickly look up the definition of words while you’re reading a PDF, and one-click save the definition as an annotation for future reference.

![](https://lh4.googleusercontent.com/CD7RkxCQQegt07oCi5LzhmBOdf0wHsYalbrEerwceD2_ajgubH9axMxA7uAk09ACRWnI7LDVLFXZAp8BVtf7HpZL3l3m5OE1BajaISuK_Kf09AfeQ6qTL7fhJrIRSiK-R1uVQQKH)

## User Stories

The following functionality is completed:
- [x] User can select any text and right click to see the definition in a dialog box
- [x] User can one-click create a highlight annotation containing the definition 
- [x] User can see selected text dynamically populated in the right-click context menu
- [x] User sees the definition for the root word if they select a variation of the word

The following improvements are pending:
- [ ] User can add annotations to any page in view, not just the active page
- [ ] Button text in the dialog box makes it clear how to add an annotation 

## Video Walkthrough

View the video walkthrough [here](https://youtu.be/8caU7Ly_IFk).

## Installation

This project uses 2 third-party dynamically linked libraries (.dll) which are provided in the Dictionary folder. You will need to copy these .dll files to the plugins directory where Foxit PhantomPDF is installed on your computer. The full step-by-step guide is [here](https://drive.google.com/file/d/1SZXN-nivu1K_pNViyLAATRgqH_g4QC_c/view?usp=sharing).

This plugin uses the Merriam-Webster Dictionary API. Request a free API key [here](https://dictionaryapi.com/register/index) and paste it into the Dictionary/keys.h file where indicated. 

## Technologies Used

Foxit PhantomPDF Plugin SDK, Microsoft C++ REST SDK, Merriam-Webster Dictionary API

## Acknowledgements

Thanks to Huy Tran from Foxit for his help with the Plugin SDK throughout this project!
