# Complete grammar
```abnf
<program>                   ::= 
                                <declarations>
<declarations>              ::= 
                                <declaration> <declarations> 
                                | ""
<declaration>               ::= 
                                <using-declaration> 
                                | <parameter-declaration> 
                                | <function-declaration>
<using-declaration>         ::= 
                                "USING" NAME "=" <using-variant> ";" 
<using-variant>             ::= 
                                "STRUCT" "{" <structure-declaration> "}"
                                |   TYPE
                                |   "RULE" TYPE  <compound-statement> 
<parameter-declaration>     ::= 
                                "PARAMETER" TYPE NAME <assign>
<assign>                    ::= 
                                "=" <literal> ";"
                                | ";" 
<function-declaration>      ::= 
                                TYPE NAME  "(" <type-list> ")" <compound-statement> 
<structure-declaration>     ::= 
                                <type-declaration> ";" 
                                | <structure-declaration> <type-declaration> ";" 
<type-list>                 ::= 
                                <type-declaration> 
                                | <type-list> "," <type-declaration>
                                | ""
<type-declaration>          ::= TYPE NAME 
              
<compound-statement>        ::= "" 
<statements>                ::= 
                                <statement> <statements> 
                                | "" 
<statement>                 ::= 
                                <call-statement> ";" 
                                | <declaration> ";" 
                                | <assignment> ";"
                                | <if-statement>  
                                | <while-statement> 
                                | <return> ";"
<declaration>               ::= TYPE NAME <declaration-end>
<declaration-end>           ::= 
                                "" 
                                |   "=" <expression> 
<call-statement>            ::= <function-call>
<function-call>             ::= NAME 
<argument-list>             ::= 
                                "" 
                                | <argument> 
                                | <argument-list> "," <argument>
<argument>                  ::= <expression>
<assignment>                ::= <structured-member>  <assignment-end>
<assignment-end>            ::= 
                                ""
                                | "=" <expression>
<if-statement>              ::= "IF" "(" <expression> ")" <compound-statement> <else-clause>
<else-clause>               ::= 
                                "ELSE" <compound-statement> 
                                | ""
<while-statement>           ::= "WHILE" "(" <expression> ")" <compound-statement>
<return>                    ::= "RETURN" <return-end> 
<return-end>                ::= 
                                "" 
                                |  <expression>
<typeid>                    ::= 
                                "<" TYPE ">" 
                                | "(" NAME ")" 
<expression>                ::= 
                                <literal>
                                |   "TYPEID" <typeid>
                                |   "CONVERT" "<" TYPE ">" "(" <expression> ")"
                                |   <function-call> 
                                |   <expression> "&&" <expression>
                                |   <expression> "||" <expression>
                                |   <expression> "<" <expression>
                                |   <expression> ">" <expression>
                                |   <expression> "==" <expression>
                                |   <expression> "!=" <expression>
                                |   <expression> "-" <expression>
                                |   <expression> "+" <expression>
                                |   <expression> "/" <expression>
                                |   <expression> "*" <expression>
                                |   "(" <expression> ")" 
                                |   "<->" <expression> 
                                |   "+" <expression> 
                                |   "!" <expression> 
<structured-member>         ::= 
                                <structured-member> "." <structured-member-end> 
                                |    NAME 
<structured-member-end> ::=
                               |    "INSERT" "(" <expression> ")"
                               |    "AT" "(" <expression> ")"
                               |    "SIZE" "(" ")"
                               |    "DEL" "(" <expression> ")"
<literal>                   ::= INTEGER 
                              | FLOAT
                              | STRING 
                              | BOOL
                              | <structured-member>
```
