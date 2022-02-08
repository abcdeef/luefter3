T_TEMP::T_TEMP(void) {
  this->val = 0;
  this->old = 0;
}

int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;


void T_TEMP::get(void) {

  Vo = analogRead(A0);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  this->val = T - 273.15;

  if (this->old != this->val) {

    String asd = "{\"TEMP\": " + String(this->val) + "}" ;
    webSocket.broadcastTXT(asd);



    this->old = this->val;
  }
}
