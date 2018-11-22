<?php
    $rutaExe = "../Codigo/proxMinimo/proxMinimoMediciones";

    $promedio = 0;
    $count = 0;
    $peor = -1;
    $mejor = 999;
    
    $sets = scandir("../Tests_Catedra");
    foreach($sets as $set){
        if($set == "." || $set == ".." ) continue;
        $tests = scandir("../Tests_Catedra/$set");
        $max = 4;
        $medicionesSet = 0;
        
        echo "(table)\n";
        
        echo "(row)\n";
        echo "\t(titlecell)Set $set(/titlecell)\n";
        echo "\t(titlecell)Coste Óptimo(/titlecell)\n";
        echo "\t(titlecell)Coste Heurístico(/titlecell)\n";
        echo "\t(titlecell)Diferencia al óptimo(/titlecell)\n";
        echo "(/row)\n";
        
        foreach($tests as $test){
            if($test == "." || $test == ".." ) continue;
            if(strpos($test, "sol") !== false) continue;
            $filebase = explode(".", $test)[0];
            $optimo_file= explode("\n", file_get_contents("../Tests_Catedra/$set/$filebase.sol"));
            $optimo = -1;
            foreach($optimo_file as $l){
                $a = explode(" ", $l);
                if(strtolower($a[0]) == "cost"){
                    $optimo = trim($a[1]);
                    break;
                }
            }
            if($optimo == -1) continue; //Skippeo los que no tengan soluciones como la gente.
            $rutaArchivo = "../Tests_Catedra/$set/$filebase.vrp";
            $res = explode("\n", shell_exec("nice -n 0 $rutaExe $rutaArchivo"));
            $coste = -1;
            foreach($res as $l){
                $a = explode(" ", $l);
                if($a[0] == "@CosteOptimizado:"){
                    
                    $coste = trim($a[1]);
                    
                    break;
                }
            }
            if($coste == -1) continue; //sE BARDEÓ
            if($coste == "inf") continue; //BARDEAUX
            $porcentaje = (100 - ($optimo * 100 / $coste));
            if($porcentaje > 80) continue; //El único que da más de 50 es un caso roto
            $promedio = $promedio + $porcentaje;
            if($porcentaje < $mejor) $mejor = $porcentaje;
            if($porcentaje > $peor) $peor = $porcentaje;
            ++$count;
            ++$medicionesSet;
            if($max > 0){
            --$max;
            echo "(row)\n";
            echo "\t(cell)$filebase(/cell)\n";
            echo "\t(cell)$optimo(/cell)\n";
            echo "\t(cell)$coste(/cell)\n";
            echo "\t(cell)" . $porcentaje . "(/cell)\n";
            echo "(/row)\n";
            }
        }
        echo "(/table)\n";
        echo "(caption)Mediciones del set $set, cuatro aleatorias de un total de $medicionesSet.(caption)\n";
    }
    
    echo "Promedio: " . ($promedio / $count) . "\n";
    echo "Mejor: " . $mejor . "\n";
    echo "Peor: " . $peor . "\n";
?>
