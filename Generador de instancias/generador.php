<?php
    // - Si no recibí tres parámetros, error -
    if(count($argv) != 5)
    {
        println("Espero dos parámetros: n y m, c1 y c2");
        println("- n para generar instancias de [3, n] elementos");
        println("- m para generar m instancias para cada i en [3, n]");
        println("La capacidad de los camiones es un entero aleatorio en [c1, c2]");
        println("La capacidad se decide aleatoriamente por instancia.");
        exit(1);
    }
    
    // - Máxima cantidad de elementos -
    $max_elems = $argv[1];
    
    // - Instancias por cantidad de elementos -
    $cant_inst = $argv[2];
    
    // - Capacidades máximas y mínimas -
    $c1 = $argv[3];
    $c2 = $argv[4];
    
    if(!file_exists("./instancias_generadas")) 
        mkdir("./instancias_generadas");
    
    // - Ciclo por cada n generando m instancias
    for($n = 3; $n <= $max_elems; ++$n)
    {
        echo "$n";
        if(!file_exists("./instancias_generadas/$n-elementos")) 
            mkdir("./instancias_generadas/$n-elementos");
        for($m = 1; $m <= $cant_inst; ++$m)
        {
            echo ".";
            // - Genero una instancia nueva -
            $instancia = "NAME : LPS-n$n-m$m\n";
            $instancia = $instancia . "COMMENT : (Lartu-Peter-Santi Plot, instance " . (($n-3) * $cant_inst + ($m)) . " out of " . (($max_elems-2) * $cant_inst) . ". $n customers.)\n";
            $instancia = $instancia . "TYPE : CVRP\n";
            $instancia = $instancia . "DIMENSION : $n\n";
            $instancia = $instancia . "EDGE_WEIGHT_TYPE : EUC_2D\n";
            $capacidad = rand($c1, $c2);
            $instancia = $instancia . "CAPACITY : $capacidad\nNODE_COORD_SECTION\n";
            $coordenadasUsadas = [];
            //Agrego puntos
            for($p = 1; $p <= $n; ++$p)
            {
                $x = rand(0, 100);
                $y = rand(0, 100);
                if(in_array([$x, $y], $coordenadasUsadas)){
                    --$p;
                    continue;
                }
                array_push($coordenadasUsadas, [$x, $y]);
                $instancia = $instancia . " $p $x $y\n";
            }
            $instancia = $instancia . "DEMAND_SECTION\n";
            //Agrego pesos
            for($p = 1; $p <= $n; ++$p)
            {
                $peso = rand(1, $capacidad);
                if($p == 1) $peso = 0;
                $instancia = $instancia . "$p $peso\n";
            }
            $instancia = $instancia . "DEPOT_SECTION\n 1\n -1\nEOF\n";
            file_put_contents("./instancias_generadas/$n-elementos/LPS-n$n-m$m.vrp", $instancia);
        }
        echo "\n";
    }
    
    // - Función que imprime un texto con salto de linea -
    function println($txt)
    {
        echo $txt . "\n";
    }
?>
