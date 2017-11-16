## Compiler

##Chamada do compilador:
	./[nome do programa] -[opcoes] [arquivo de entrada] [arquivo de saida]
	
	[opcoes]:
		i: escrever codigo intermediario no arquivo de saida
		
##Sintaxe da linguagem:

>/*	assim se
>	faz
>	um
>	comentario de multiplas linhas
>*/
>int b <- 2
>int c;

>a <- 2 + 3	//tipo de a sera inferido
>c <- a+b*2

>if a < b do:
>	print a
>else if a = b do:
>	print c
>end

>if b > 0 do:
>	for i from 10 to 0 do:	//variavel de contagem nao precisa ser declarada previamente
>		print i
>	end
>else for i stepping 2 from 0 to 20 do:	//diferente, nao?
>	print i
>end

>while c > b do:
>	print c
>	c <- c-1
>end

