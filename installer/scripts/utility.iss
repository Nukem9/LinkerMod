[Code]
(* UTILITY FUNCTION - REMOVE FOR PRODUCTION *)
function BoolToStr(Value : Boolean) : String; 
begin
  if Value then
    result := 'true'
  else
    result := 'false';
end;