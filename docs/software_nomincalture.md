## **Somax Software API Nominclature**
---------------------
&nbsp;


The following describes the Somax naming convention.

* Word Case
   * preprocessor constants and structures are uppercase.
   * Everything else is lower case.
* Word Separation
   * words are separated using underscores.
* Function Names AAAA_BBBB_CCCC
   * AAAA is a short name for the file where the function is located.
   * BBBB is of several types
      * nit - Initalization and deinitialization. Create or destroy a context.
      * cfg - Configuration. Modify static and/or dynamic context properites.
      * inf - Information. Read but do not modify static and/or dynamic properties
      * opr - Operate. Do something germaine to context purpose.