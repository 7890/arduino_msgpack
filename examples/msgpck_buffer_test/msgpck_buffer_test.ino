byte * bin;

//=============================================================================
void setup() {
  msg_init();

  //byte * 
  bin=new byte[3];
  bin[0]='a';
  bin[1]='b';
  bin[2]='c';

  Serial.begin(115200);

  Serial.write(192);
  Serial.print("hello");
  Serial.write(192);
}

//=============================================================================
void loop() {
/*
  msg_add_s("fooo");
  msg_add_c(0xc0); //SLIP_END (should be escaped)
  msg_add_T();
  msg_add_F();
  msg_add_N();
  msg_add_I();
  msg_add_i(1234);
  msg_add_h((1234L);
  msg_add_f(0.77f);
  msg_send_short(42);
*/
  test();
  delay(5);
}

//=============================================================================
void test()
{
  int32_t min_i=-2147483648;
  int32_t max_i=2147483647;
  int64_t min_h=-9223372036854775808;
  int64_t max_h=9223372036854775807;
  float min_f=-3.40282347E+38f;
  float max_f=3.40282347E+38f;
  float small_f=1.175494351E-38f;

  msg_new();
  msg_add_N();
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_T();
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_F();
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_I();
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_s("a");
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_i(0);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_i(min_i);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_i(max_i);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_h(0);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_h(min_h);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_h(max_h);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_f(0);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_f(min_f);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_f(max_f);
  msg_send("/");
  msg_send("/abc");

  msg_new();
  msg_add_f(small_f);
  msg_send("/");
  msg_send("/abc");

/*
  double min_d=-1.797693E+308d;
  double max_d=1.797693E+308d;
  double small_d=2.225074E-308d;
*/
}//end test()
//EOF
