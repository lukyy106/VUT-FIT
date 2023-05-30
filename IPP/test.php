<?php
// author Lukas Neupauer
// login xneupa01
// prgram IPP project 2021/2022 test.php


// arguments
$directory = ".";
$recursive = false;
$pase = "parse.php";
$interpret = "interpret.py";
$parse_only = false;
$interpret_only = false;
$jexamxml = "/pub/courses/ipp/jexamxml/jexamxml.jar";
$no_clean = false;

// content of the page
$doc = [];



date_default_timezone_set("Europe/Prague");


// parsing arguments
foreach($argv as $x){
  if($x == "--help"){
    echo "arguments\n --directory=file \n --parse-script=file \n --int-script=file \n --parse-only \n --int-only \n --jexampath=file \n --noclean";
    exit(0);
  }elseif(preg_match("/--directory=.+/", $x)){
    $directory = str_replace("--directory=", "", $x);
  }elseif($x == "--recursive"){
    $recursive = true;
  }elseif(preg_match("/--parse-script=.+/", $x)){
    $parse = str_replace("--parse-script=", "", $x);
  }elseif(preg_match("/--int-script=.+/", $x)){
    $interpret = str_replace("--int-script=", "", $x);
  }elseif($x == "--parse-only"){
    $parse_only = true;
  }elseif($x == "--int-only"){
    $interpret_only = true;
  }elseif(preg_match("/--jexampath=.+/", $x)){
    $jexamxml = str_replace("--jexampath=", "", $x);
  }elseif($x == "--noclean"){
    $no_clean = true;
  }elseif($x == "test.php"){ // name of this script

  }else{
    fwrite(STDERR, "Invalid argument $x" . PHP_EOL);
    exit(10);
  }
}





// print head of page
echo "<!DOCTYPE html>";
echo "<html>";
echo "<head>";
echo "<title>IPP test</title>";
echo "<meta charset='utf-8'/>";
echo "<style>";
echo "* {margin:0; padding:0;}";
echo "body {box-sizing: border-box; font-size: 0; width: 100%; background-color: whitesmoke; position: fixed; height: 100%; display: inline-block;}";
echo "header {grid-template-columns: max-content auto; font-size: 20px; padding-left: 5px; padding-right: 5px; width:100%; background-color:#333333; color: white; vertical-align: middle; display: grid}";
echo "h1 {dysplay: block; vertical-align: middle;}";
echo " header p {dysplay: block; padding-left: 15px;}";
echo "div {display: block;}";
echo "section {display: inline_block; font-size: 16px; overflow-y: scroll; width: 100%; height: 100%}";
echo "section div {padding: 10px; display: flex; justify-content: center; align-items: center; width: 100%;}";
echo "section div div {display: block; width: 80%; background-color: white; border-radius: 25px}";
echo "section div div h1 {vertical-align: middle; font-size: 26px; float:left; width: 5%;}";
echo "section div div p {vertical-align: middle; font-size: 20px; float:left;}";
echo "</style>";
echo "</head>";
echo "<body>";
echo "<header>";
echo "<h1>";
echo "IPP test results";
echo "</h1>";
echo "<p style=\"float: right;\">";
echo "Made <br>" . date("G:i:s d.m.Y");
echo "</p>";
echo "</header>";
echo "<section>";
echo "<p style=\"color: black; padding-bottom: 100px\">";

if($recursive){
  exec("find $directory -name '*.src'", $output, $ret_val);
}else{
  exec("find $directory -maxdepth 1 -name '*.src'", $output, $ret_val);
}
if($ret_val != 0){
  exit(41);
}

if($parse_only and $interpret_only){
  fwrite(STDERR, "Invalid argument combination" . PHP_EOL);
  exit(10);
}

// number of tests + 1
$index = 1;
// number of successful tests + 1
$success_count = 1;

foreach($output as $k){
  // error flag
  $error = false;
  // return value from shell command
  $result = 1;
  // output from shell comand
  $return = [];
  array_push($doc, "<div>");
  array_push($doc, "<div>");
    // file for temporary data
    $save_to = str_replace(".src", ".out_tmp", $k);
    // file with expected output
    $compare_with = str_replace(".src", ".out", $k);
    // file with expected return value
    $expected_return_file = str_replace(".src", ".rc", $k);
    // file with inut data
    $input = str_replace(".src", ".in", $k);

    if(!file_exists($compare_with)){
      exec("touch $compare_with");
    }
    if(!file_exists($input)){
      exec("touch $input");
    }
    if(!file_exists($expected_return_file)){
      exec("print 0 >$expected_return_file");
    }

    if(!$interpret_only){
      // retv_val - returned value from script
      exec("php $parse  <$k >$save_to", $dump, $ret_val);
      if($ret_val == 1){
        exit(41);
      }
      if($parse_only){
        if($ret_val == 0){
            exec("java -jar $jexamxml $save_to $compare_with options", $output, $result);
        }
        exec("cat $expected_return_file", $return, $dump);
        if($return[0] == $ret_val){
          $result = 0;
        }
      }else{
        if($ret_val != 0){
          continue;
        }else{
          $result = 0;
        }
      }

    }elseif(!$parse_only and !$error){
      exec("python3 $interpret  --source=$k --input=$input >$save_to", $dump, $ret_val);
      if($ret_val == 1){
        exit(41);
      }
      $result = 1;
      $return = [];
      if($ret_val == 0){
          exec("diff $save_to $compare_with", $output, $result);
      }
      exec("cat $expected_return_file", $return, $dump);
      if($return[0] == $ret_val){
        $result = 0;
      }
    }

    if(!$no_clean){
      exec("rm $save_to");
    }




  array_push($doc, "<h1>" . $index . "</h1>");
  array_push($doc, "<p style=\"height: 100%; padding-top: 4px\">");
  array_push($doc, "<b>Test</b></p>");
  array_push($doc, "<p style=\"padding-left: 30px; padding-top: 4px\">  " . str_replace(".src", "", $k) . "</p>");
  array_push($doc, "<p style=\"font-size: 30px; float: right\">");
  if($result == 0){
    array_push($doc, "<span style=\"color: green\">Pass</span>");
    $success_count += 1;
  }else{
    array_push($doc, "<span style=\"color: red\">Fail</span>");
  }
  array_push($doc, "</p>");
  if($result != 0){
    array_push($doc, "<p style=\"width: 70%; font-size: 16px; padding-top: 4px; float: left\">" . "Expected $return[0] but recived $ret_val" . "</p>");
  }
  array_push($doc, "</div>");
  array_push($doc, "</div>");
  $index += 1;


}

array_push($doc, "</p>");
array_push($doc, "<div style=\"width: 100%; height: 20%\"></div>");
array_push($doc, "</section>");


array_push($doc, "</body>");
array_push($doc, "</html>");


// print testing details
echo "<div style=\"width: 100%; height: 100px;\">";
echo "<div style=\"height: 100px;\">";
echo "<p style=\"width: 50%\"><b>File</b> $directory";
if($recursive){
  echo "<br><b>Recursive</b>";
}
if($parse_only){
  echo "<br><b>Parse only</b>";
}
if($interpret_only){
  echo "<br><b>Interpret only</b>";
}
if($no_clean){
  echo "<br><b>Keep temporary files</b>";
}
echo "</p>";
echo "<p style=\"display: block; width: 20%; padding-top: 20px; text-align: right; font-size: 50px; float: right\">";
if($index == 1){
  echo "0%";
}else{
  echo round(($success_count-1)*100/($index-1)) . "%";
}

echo "</p>";


// print results from testing
echo "<p style=\"display: block; width: 20%;text-align: left; float: right\">";
echo "<b>Tests</b> " . $index-1;
echo "<br><b>Passed</b> " . $success_count-1;
echo "<br><b>Failed</b> " . $index - $success_count;

echo "</p>";

echo "</div>";
echo "</div>";

// print page
foreach($doc as $k){
  echo $k;
}

?>
