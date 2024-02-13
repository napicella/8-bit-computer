void main() {
    int volatile * const p_reg = (int *) 0x8002;
    *p_reg = 0xff;

    while (1==1)
    {
        int volatile * const p_reg = (int *) 0x8000;
        *p_reg = 0xff;
    }
}