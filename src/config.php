<?php
function myconfig($filename){
    $ret = array();
    $lines = file($filename);
    foreach ($lines as $line_num => $line) {
        $line = str_replace("\n", "", $line);
        $line = str_replace("\r", "", $line);
        if($line[0] == '#' || $line[0] == ';' || $line == '') continue;
        $line_element = explode("=", $line);
        $ret[$line_element[0]] = $line_element[1];
    }
    return $ret;
}
