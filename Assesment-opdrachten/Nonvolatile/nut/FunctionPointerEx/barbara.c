/* babara() is in fact a generic function caller */

void barbara ( void (*function_ptr)() )
{
  /* Call one of my functions */

  (*function_ptr)();

}