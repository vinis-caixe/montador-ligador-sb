;Código fonte do
;módulo A:
MOD_A: BEGIN
Y: EXTERN
MOD_B: EXTERN
PUBLIC VAL
PUBLIC L1
SECAO TEXTO
INPUT Y
LOAD VAL
ADD Y
STORE Y
JMPP MOD_B
L1: STOP
SECAO DADOS
VAL: CONST 5
END