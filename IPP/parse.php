<?php
// author Lukas Neupauer
// date march 2022
// project FIT VUT IPP project 1 - parser 

// all statistic about parsed program
class Statistic{
  // number of parsed instructions
  public $instruction_order = 0;
}
// all universal functions of instructions needed for parsing
class Instruction{
  // syntactic check of name of variable or lable
  // @text name of variable or lable
  // return true if name is syntacticlly incorrect and false if correct
  public function regex_check($text){
    if(preg_match("/[^a-zA-Z0-9?\_\-$\%\*!&]/", $text) || !preg_match("/\A[a-zA-Z?\_\-$\%\*!&]/", $text)){
      return(true);
    }
    return(false);
  }
  // printing instruction and call function that print its arguments
  // @arguments arguments of printed instructoin but the zeroth argument is the name of the instruction, type array
  // @types types of arguments
  // @instruction_order order of instruction that is parsed
  // no return
  public function print_instruction($arguments, $types, $instruction_order){
    // replacin problematig characters for xml
    for($i=0; $i < count($arguments); $i++){
      $arguments[$i] = str_replace('&', '&amp;', $arguments[$i]);
      $arguments[$i] = str_replace('<', '&lt;', $arguments[$i]);
      $arguments[$i] = str_replace('>', '&gt;', $arguments[$i]);
    }
    echo("\t<instruction order=\"$instruction_order\" opcode=\"" . $arguments[0] . "\">\n");
    for($i=1; $i < count($arguments); $i++){
      $this->print_arguments($arguments[$i], $types[$i-1], $i);
    }
    echo("\t</instruction>\n");
    //$this->instruction_order += 1;
  }
  // printing arguments of instruction
  // @argument argument
  // @type type of argument
  // @number order of argument in instruction
  // no return
  private function print_arguments($argument, $type, $number){
    echo("\t\t<arg$number type=\"$type\">$argument</arg$number>\n");
  }
}

// class for parsing variables that extends class Instruction
class Variable extends Instruction{
  // @argument_types types of arguments in instruction
  public $argument_types;
  // @is_variable flag that shows if argument is variable or not
  public $is_variable;
  // checks if arguemnt is variable and set @is_variable on true or false
  // @var arguemnt that can by variable
  // no return
  public function var_check($var){
    $variable = explode('@', $var);
    if(count($variable) != 2){
      $this->is_variable = false;
    }
    elseif($variable[0] != "GF" && $variable[0] != "TF" && $variable[0] != "LF"){
      $this->is_variable = false;
    }
    elseif($this->regex_check($variable[1])){
      $this->is_variable = false;
    }
    else{
      $this->is_variable = true;
    }
  }
}
// class for parsing symbols that extends class Variable
class Symb extends Variable{
  // @is_constant flag that shows if argument is constant or not
  public $is_constant;
  // checks if arguemnt is constant and set @is_constant on true or false
  // @const argument that can by constant
  // no return
  public function const_check($const){
    $this->is_constant = true;
    $const = explode('@', $const, 2);
    if(count($const) != 2){
      $this->is_constant = false;
    }
    if(!in_array($const[0], Key_words::DATA_TYPES)){
      $this->is_constant = false;
    }
    if($const[1] == "" && $const[0] != "string"){
      $this->is_constant = false;
    }
    if(preg_match("/\\\\(?!([0-9][0-9][0-9]))/", $const[1])){ // looking for '/' character except for escape sequences
      $this->is_constant = false;
    }
    if($const[0] == "nil" && $const[1] != "nil"){
      $this->is_constant = false;
    }
    if($const[0] == "int"){
        if(!preg_match("/\d+/", $const[1])){
          $this->is_constant = false;
        }
    }
    if($const[0] == "bool"){
      if($const[1] != "true" && $const[1] != "false"){
        $this->is_constant = false;
      }
    }
  }
}


// key words of parsed language
class Key_words{
  // names of instructions and types of thers arguemnts
   const INSTRUCTIONS = [
                    "MOVE" => ["var", "symb"],
                    "CREATEFRAME" => [],
                    "PUSHFRAME" => [],
                    "POPFRAME" => [],
                    "DEFVAR" => ["var"],
                    "CALL" => ["label"],
                    "RETURN" => [],
                    "PUSHS" => ["symb"],
                    "POPS" => ["var"],
                    "ADD" => ["var", "symb", "symb"],
                    "SUB" => ["var", "symb", "symb"],
                    "MUL" => ["var", "symb", "symb"],
                    "IDIV" => ["var", "symb", "symb"],
                    "LT" => ["var", "symb", "symb"],
                    "GT" => ["var", "symb", "symb"],
                    "EQ" => ["var", "symb", "symb"],
                    "AND" => ["var", "symb", "symb"],
                    "OR" => ["var", "symb", "symb"],
                    "NOT" => ["var", "symb"],
                    "INT2CHAR" => ["var", "symb"],
                    "STRI2INT" => ["var", "symb", "symb"],
                    "READ" => ["var", "type"],
                    "WRITE" => ["symb"],
                    "CONCAT" => ["var", "symb", "symb"],
                    "STRLEN" => ["var", "symb"],
                    "GETCHAR" => ["var", "symb", "symb"],
                    "SETCHAR" => ["var", "symb", "symb"],
                    "TYPE" => ["var", "symb"],
                    "LABEL" => ["label"],
                    "JUMP" => ["label"],
                    "JUMPIFEQ" => ["label", "symb", "symb"],
                    "JUMPIFNEQ" => ["label", "symb", "symb"],
                    "EXIT" => ["symb"],
                    "DPRINT" => ["symb"],
                    "BREAK" => []
  ];
  // data types of parsed language
  const DATA_TYPES = ["string", "int", "bool", "nil"];
  // frames of parsed language
  const FRAMES = ["GF", "LF", "TF"];

}

// suported arguments of program
if (in_array("--help", $argv)){
  if(count($argv) > 2){
    exit(10);
  }
  echo "Usage of program\n\tphp parse.php [parameters] < source_file \nPrameters\n\t--help - print this message. \n";
  exit(0);
}
// @missing_header flag that shows if sourse program cointain program hrader
$missing_header = true;
// @var declaration of object variable for work with variables
$var = new Variable;
// @symb declaration of objct variable for work with variable or constants
$symb = new Symb;
// @stat declaration of objct variable for measuring statistics
$stat = new Statistic;

// main loop of program
while(true){
  if(feof(STDIN)){
    break;
  }
  $read_line = trim(fgets(STDIN), "\n"); // read one line from source file a remove new line character
  $exploded_read_line = explode("#", $read_line); // devide code from comments
  $read_line = $exploded_read_line[0]; // remove comments from code
  $read_line = str_replace("\t", " ", $read_line); // replace all tabulators by spaces
  $read_line = ltrim($read_line); // remove white characters from beginning
  $read_line = rtrim($read_line); // remove white characters from end
  if($missing_header){
    $read_line = strtolower($read_line); // lower case of all characters
    if($read_line == ".ippcode22"){ // found header
      $missing_header = false;
      echo("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      echo("<program language=\"IPPcode22\">\n");
      continue;
    }else{
      if($read_line != ""){
        exit(21); // missing header
      }
    }
  }else{
    $read_line = preg_replace("/ {2,}/", " ", $read_line); // remove multiple spaces nex to eachothers by single include_once
    $read_line = explode(" ", $read_line); // devide line by  spaces to array
    $read_line[0] = strtoupper($read_line[0]); // set opcode to low case
  }
  if($read_line == ""){
    continue;
  }
  if(in_array($read_line[0], array_keys(Key_words::INSTRUCTIONS))){ // check if name of instruction is valid
    if(count($read_line) != 1+count(Key_words::INSTRUCTIONS[$read_line[0]])){ // check if number of argument is valid
      exit(23); // missing or too much arguments
    }
  }else{
    if($read_line[0] == ""){
      continue;
    }
    exit(22); // unknown instruction
  }
  $var->argument_types = []; // declare variable or remove old data
  for($i = 1; $i <= count(Key_words::INSTRUCTIONS[$read_line[0]]); $i++){ // loop for checking arguments of instruction
    switch(Key_words::INSTRUCTIONS[$read_line[0]][$i-1]){
      case "var":
        $var->var_check($read_line[$i]);
        if(!$var->is_variable){
          exit(23);
        }
        array_push($var->argument_types, "var");
        break;
      case "symb":
        $symb->var_check($read_line[$i]);
        if(!$symb->is_variable){
          $symb->const_check($read_line[$i]);
          if(!$symb->is_constant){
            exit(23);
          }
          array_push($var->argument_types, explode('@', $read_line[$i], 2)[0]);
          $read_line[$i] = explode('@', $read_line[$i], 2)[1];
        }else{
          array_push($var->argument_types, "var");
        }
        break;
      case "label":
        if($var->regex_check($read_line[$i])){
          exit(23);
        }
        array_push($var->argument_types, "label");
        break;
      case "type":
        if(!in_array($read_line[$i], Key_words::DATA_TYPES)){
          exit(23);
        }
        array_push($var->argument_types, "type");
        break;
    }
  }
  $stat->instruction_order++;
  $var->print_instruction($read_line, $var->argument_types, $stat->instruction_order);

}

if($missing_header){
  exit(21);
}

echo "</program>\n";

exit(0);


?>
