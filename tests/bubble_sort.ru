 fn bubbleSort(arr) {
        for(set i := 0; i < len(arr); i := i + 1) {
            for(set j := 0; j < len(arr)-i-1; j := j + 1) {
                if(arr[j] > arr[j+1]) {
                    set tmp := arr[j];
                    arr[j] := arr[j+1];
                    arr[j+1] := tmp;
                }
            }
        }
        return arr;
    }


fn main(args){

    set arr := [21, 5, 8, 13, 12, 69, 96, 34];
    set arr := bubbleSort(arr);
    for(set i := 0; i < len(arr); i++){
        brint (arr[i]);
    }
  return 0;
}
