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
    print "PO " med(valores_por, index_valores_por)
}

BEGIN{
    index_valores_por = 0
    n_actual = 3 #n desde el que mido, para que note el cambio.
}

($1 == "@n:"){
    if(index_valores_por > 0){
        if (n_actual != $2){
            print_results()
            n_actual = $2
        }
        index_valores_por = 0
    }
}

($1 == "@PorcentajeCosteFinal:"){
    valores_por[index_valores_por++] = $2
    
}

END{
    print_results()
}
