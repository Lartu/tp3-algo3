#Analizador de resultados
function med(v, t){
    res = 0
    for(a = 0; a < t; a++){
        res+=v[a];
    }
    res = res / a
    return res
}

function print_results(){
    print "N " n_actual
    print "CA " med(valores_can, index_valores_can)
    print "OP " med(valores_opt, index_valores_opt)
}

BEGIN{
    index_valores_can = 0
    index_valores_opt = 0
    n_actual = 3 #n desde el que mido, para que note el cambio.
}

($1 == "@n:"){
    if(index_valores_can > 0){
        if (n_actual != $2){
            print_results()
            n_actual = $2
        }
        index_valores_can = 0
        index_valores_opt = 0
    }
}

($1 == "@CosteCanonico:"){
    valores_can[index_valores_can++] = $4
    
}

($1 == "@CosteOptimizado:"){
    valores_opt[index_valores_opt++] = $4
}

END{
    print_results()
}
