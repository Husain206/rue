#include <cstdlib>
set arr := [" "," ", " ",
             " ", " ", " ",
             " ", " ", " "];

set currentPlayer := "x";

fn print_board(){
  brint ("\n-----------");
  brint (arr[0], " |", arr[1], "| ", arr[2]);
  brint ("---+---+---");
  brint (arr[3], " |", arr[4], "| ", arr[5]);
  brint ("---+---+---");
  brint (arr[6], " |", arr[7], "| ", arr[8]);
  brint ("-----------\n");
}

fn checkMove(inpt) {
   if(inpt >= 0 && inpt < 9 && arr[inpt] == " "){
       return true;
     
   }
}

fn checkDraw(){
  for(set i :=0; i<9; i++){
    if(arr[i] == " "){
      return false;
    }
  }
  return true;
}

 
fn checkWin(player) {
  
  if (arr[0] == player && arr[4] == player && arr[8] == player) { return true; }
  if (arr[2] == player && arr[4] == player && arr[6] == player) { return true; }

  for (set i := 0; i < 9; i := i + 3) {
    if (arr[i] == player && arr[i + 1] == player && arr[i + 2] == player) { return true; } 
  }

  for (set i := 0; i < 3; i++) {
    if (arr[i] == player && arr[i + 3] == player && arr[i + 6] == player) { return true; }
  }

 return false;
}

fn reset(){
  set x := inbut("do you want to continue? (y/n): ");
      if(x == "y"){
        currentPlayer := "x";
        for(set i := 0; i < 9; i++){
          arr[i] := " ";
        }
      } else {
        exit(0);
  }
}

fn play(){
  ala(true){
    if(checkDraw()){
      print_board();
      brint("\n  ---|!Draw!|---\n");
      reset();
    }
    print_board();
    brint(currentPlayer, "'s turn");
    set move := inbut("enter move: (0-8) ");

    if(!checkMove(move)){
      brint("invalid move, try again");
      exit(-1);
    }

    

    arr[move] := currentPlayer;

    if(checkWin(currentPlayer)){
      print_board();
      brint(currentPlayer, "won");
      reset();
    }

    currentPlayer := (currentPlayer == "x") ? "o" : "x";
  }
}


play();
