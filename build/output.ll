; ModuleID = 'main'
source_filename = "main"

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br i1 true, label %if, label %merge

if:                                               ; preds = %entry
  %y = alloca i32, align 4
  store i32 0, ptr %y, align 4
  br label %merge

merge:                                            ; preds = %if, %entry
  ret i32 0
}
