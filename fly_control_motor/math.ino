void itob(void *dest, size_t dest_len, int32_t src)
  {
    size_t i;
    /* Copy bytes from int to array */
    for (i = 0; (i < dest_len) && (i < sizeof(int32_t)); ++i)
    {
      ((uint8_t *) dest)[i] = (src >> (i * 8)) & 0xff;
    }
    /* If array is larger than int, sign-fill the remainder */
    for (; i < dest_len; ++i)
    {
      if (((uint8_t *) dest)[dest_len - 1] & 0x80)
      { // MSB is set, int is negative
        ((uint8_t *) dest)[i] = 0xff;
      }
      else
      { // int is positive
        ((uint8_t *) dest)[i] = 0;
      }
    }
  }
  int32_t btoi(void *src, size_t src_len)
  {
    int32_t retval = 0;
    size_t i = sizeof(int64_t);

    if (!src_len) { return 0; }

    /* If array is shorter than int, need to propagate sign bit */
    for (; i >= src_len; --i)
    {
      retval = retval << 8;
      if (((uint8_t *) src)[src_len - 1] & 0x80)
      {  // MSB is set, int is negative
        retval |= 0xff;
      }
    }
    do
    {
      retval = retval << 8;
      retval |= ((uint8_t *) src)[i];
    } while (i--);

    return retval;
  }

  double btof(void *src, size_t src_len, double scale)
  {
    double retval = btoi(src, src_len);
    return retval /= scale;
  }
  void ftob(void *dest, size_t dest_len, double src, double scale)
  {
    int64_t int_src = (src * scale);
    itob(dest, dest_len, int_src);
  }
