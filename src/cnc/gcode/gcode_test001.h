const char PROGMEM gcode_file[] = "\
N1 T1M6\n\
N2 G17\n\
N3 G21 (mm)\n\
N4 (S8000)\n\
N5 (M3)\n\
N6 G92X0.327Y-33.521Z-1.000\n\
N7 G0Z4.000\n\
N8 F1200\n\
N9 G1X0.327Y-33.521\n\
N10 G1Z-1.000\n\
N11 X0.654Y-33.526\n\
N12 X0.980Y-33.534\n\
N13 X1.304Y-33.546\n\
N14 X1.626Y-33.562\n\
N15 X1.946Y-33.580\n\
N16 X2.262Y-33.602\n\
N17 X2.574Y-33.628\n\
N18 X2.882Y-33.656\n\
N19 X3.185Y-33.688\n\
N20 X3.483Y-33.724\n\
N21 X3.775Y-33.762\n\
N22 X4.060Y-33.805\n\
N23 X4.339Y-33.850\n\
N24 X4.610Y-33.898\n\
N25 X4.874Y-33.950\n\
N26 X5.130Y-34.005\n\
N27 X5.376Y-34.064\n\
N28 X5.614Y-34.125\n\
N29 X5.842Y-34.190\n\
N30 X6.060Y-34.257\n\
N31 X6.268Y-34.328\n\
N32 X6.466Y-34.402\n\
N33 X6.652Y-34.479\n\
N34 X6.827Y-34.559\n\
N35 X6.990Y-34.642\n\
N2486 G0Z4.000\n\
N2487 G0X37.560Y12.327Z6.000\n\
N2488 G1Z-1.000\n\
N2489 G1Y0.876\n\
N2490 X49.011\n\
N2491 Y12.327\n\
N2492 X37.560\n\
N2493 G0Z4.000\n\
N2494 G0Y0.876\n\
N2495 G1Z-1.000\n\
N2496 G1Y-10.575\n\
N2497 X49.011\n\
N2498 Y0.876\n\
N2499 X37.560\n\
N2500 G0Z4.000\n\
N2501 G0X49.011Y12.327\n\
N2502 G1Z-1.000\n\
N2503 G1X52.084Y15.011\n\
N2504 G0Z4.000\n\
N2505 G0X49.011Y0.876\n\
N2506 G1Z-1.000\n\
N2507 G1X52.084Y6.213\n\
N2508 Y15.011\n\
N2509 X43.286\n\
N2510 X37.560Y12.327\n\
N2511 G0Z4.000\n\
N2512 G0X49.011Y-10.575\n\
N2513 G1Z-1.000\n\
N2514 G1X52.084Y-2.585\n\
N2515 Y6.213\n\
N2516 X49.011Y0.876\n\
N2517 G0Z4.000\n\
N2518 G0X0.327Y-33.521Z-1.000\n\
N2519 M30";
