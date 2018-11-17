<?php
    $maxn = 100;
    $maxm = 100;
    $rutaExe = "../Codigo/proxMinimo/proxMinimoMediciones";

    //Verifico que todos los archivos existan
    echo "Revisando que existan todos los archivos...\n";
    for($n = 3; $n <= $maxn; ++$n){
        $carpeta = "$n-elementos";
        for($m = 1; $m <= $maxm; ++$m){
            $archivo = "LPS-n$n-m$m.vrp";
            $rutaArchivo = "../Instancias_Aleatorias/$carpeta/$archivo";
            if(!file_exists($rutaArchivo)){
                echo "$rutaArchivo no existe!\n";
                exit(1);
            }
        }
    }
    
    //Ejecuto mediciones
    echo "Ejecutando mediciones...\n";
    $mediciones = "";
    for($n = 3; $n <= $maxn; ++$n){
        $carpeta = "$n-elementos";
        for($m = 1; $m <= $maxm; ++$m){
            $archivo = "LPS-n$n-m$m.vrp";
            $rutaArchivo = "../Instancias_Aleatorias/$carpeta/$archivo";
            echo "Midiendo n: $n m: $m\n";
            $mediciones = $mediciones . "@n: $n @m: $m\n";
            $mediciones = $mediciones .  shell_exec("nice -n 0 $rutaExe $rutaArchivo");
        }
    }
    
    //Guardo el resultado
    file_put_contents("resultados_aleatorios.txt", $mediciones);
    echo "Listo!\n";
?>
