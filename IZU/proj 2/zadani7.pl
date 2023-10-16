result(D1, D) :- D1 >= 0, D is D1.
result(D1, D) :- D1 < 0, D is -D1.

sign([], P, M, D) :- D1 is P - M, result(D1, D).
sign([X|Y], P, M, D) :- X is 0, sign(Y, P, M, D).
sign([X|Y], P, M, D) :- X > 0, PP is P + 1, sign(Y, PP, M, D).
sign([X|Y], P, M, D) :- X < 0, MM is M + 1, sign(Y, P, MM, D).

uloha7([X|Y], D) :- X is 0, sign(Y, 0, 0, D).
uloha7([X|Y], D) :- X > 0, sign(Y, 1, 0, D).
uloha7([X|Y], D) :- X < 0, sign(Y, 0, 1, D).
