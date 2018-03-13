# Complete grammar
```ebnf
<PROGRAM>                   ::= <DECLARATIONS>
<DECLARATIONS>              ::= <DECLARATION> <DECLARATIONS> 
                                | ""
<DECLARATION>               ::= <USING-DECLARATION> 
                                | <PARAMETER-DECLARATION> 
                                | <FUNCTION-DECLARATION>
<USING-DECLARATION>         ::= USING NAME "=" <USING-VARIANT> ";" 
<USING-VARIANT>             ::=	STRUCT "{" <STRUCTURE-DECLARATION> "}"
                                |   TYPE
                                |   RULE TYPE  <COMPOUND-STATEMENT> 
<PARAMETER-DECLARATION>     ::= PARAMETER TYPE NAME <ASSIGN>
<ASSIGN>                    ::= "=" <LITERAL> ";"
			                    | ";" 
<FUNCTION-DECLARATION>      ::= TYPE NAME  "(" <TYPE-LIST> ")" <COMPOUND-STATEMENT> 
<STRUCTURE-DECLARATION>     ::= <TYPE-DECLARATION> ";" 
                                | <STRUCTURE-DECLARATION> <TYPE-DECLARATION> ";" 
<TYPE-LIST>                 ::= <TYPE-DECLARATION> 
                                | <TYPE-LIST> "," <TYPE-DECLARATION>
                                | ""
<TYPE-DECLARATION>          ::= TYPE NAME 
              
<COMPOUND-STATEMENT>	    ::= "" 
<STATEMENTS>                ::= <STATEMENT> <STATEMENTS> 
                                | "" 
<STATEMENT>                 ::= <CALL-STATEMENT> ";" 
                                | <DECLARATION> ";" 
                                | <ASSIGNMENT> ";"
                                | <IF-STATEMENT>  
                                | <WHILE-STATEMENT> 
                                | <RETURN> ";"
<DECLARATION>               ::= TYPE NAME <DECLARATION-END>
<DECLARATION-END>           ::= "" 
                                |   "=" <EXPRESSION> 
<CALL-STATEMENT>            ::= <FUNCTION-CALL>
<FUNCTION-CALL>             ::= NAME 
<ARGUMENT-LIST>             ::= "" 
                                | <ARGUMENT> 
                                | <ARGUMENT-LIST> "," <ARGUMENT>
<ARGUMENT>                  ::= <EXPRESSION>
<ASSIGNMENT>                ::= <STRUCTURED-MEMBER>  <ASSIGNMENT-END>
<ASSIGNMENT-END>		    ::= ""
		 	                    | "=" <EXPRESSION>
<IF-STATEMENT>              ::= IF "(" <EXPRESSION> ")" <COMPOUND-STATEMENT> <ELSE-CLAUSE>
<ELSE-CLAUSE>               ::= ELSE <COMPOUND-STATEMENT> 
			                    | ""
<WHILE-STATEMENT>           ::= WHILE "(" <EXPRESSION> ")" <COMPOUND-STATEMENT>
<RETURN>                    ::=  RETURN <RETURN-END> 
<RETURN-END>                ::=  "" 
                                |  <EXPRESSION>
<TYPEID>			        ::= "<" TYPE ">" 
	   		                    | "(" NAME ")" 
<EXPRESSION>                ::= <LITERAL>
                                |   TYPEID <TYPEID>
                                |   CONVERT "<" TYPE ">" "(" <EXPRESSION> ")"
                                | 	<FUNCTION-CALL> 
                                |   <EXPRESSION> "&&" <EXPRESSION>
                                |   <EXPRESSION> "||" <EXPRESSION>
                                |   <EXPRESSION> "<" <EXPRESSION>
                                |   <EXPRESSION> ">" <EXPRESSION>
                                |   <EXPRESSION> "==" <EXPRESSION>
                                |   <EXPRESSION> "!=" <EXPRESSION>
                                |   <EXPRESSION> "<->" <EXPRESSION>
                                |   <EXPRESSION> "+" <EXPRESSION>
                                |   <EXPRESSION> "/" <EXPRESSION>
                                |   <EXPRESSION> "*" <EXPRESSION>
                                |   "(" <EXPRESSION> ")" 
                                |   "<->" <EXPRESSION> %<PREC> UMINUS
                                |   "+" <EXPRESSION> %<PREC> UPLUS
                                |   "!" <EXPRESSION> %<PREC> NEGATION
<STRUCTURED-MEMBER>         ::=   <STRUCTURED-MEMBER> "." <STRUCTURED-MEMBER-END> 
                                |    NAME 
<STRUCTURED-MEMBER-END> ::=
                               |	INSERT "(" <EXPRESSION> ")"
                               |	AT "(" <EXPRESSION> ")"
                               |	SIZE "(" ")"
                               |    DEL "(" <EXPRESSION> ")"
<LITERAL>                   ::= INTEGER 
                              | FLOAT
                              | STRING 
                              | BOOL
                              | <STRUCTURED-MEMBER>
```
