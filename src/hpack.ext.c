#include <nghttp2/nghttp2.h>

#define DEFAULT_MAX_BUFLEN 4096

void hpackQ___ext_init__() {
}

struct hpackQ_DeflaterG_class;

struct hpackQ_Deflater {
    struct hpackQ_DeflaterG_class *$class;
    nghttp2_hd_deflater *deflater;
};

B_NoneType hpackQ_DeflaterD___init__(hpackQ_Deflater self) {
    nghttp2_hd_deflate_new(&self->deflater, DEFAULT_MAX_BUFLEN);
    return B_None;
}

B_NoneType hpackQ_DeflaterD___del__(hpackQ_Deflater self) {
    nghttp2_hd_deflate_del(self->deflater);
    return B_None;
}

B_bytes hpackQ_DeflaterD_deflate(hpackQ_Deflater self, B_dict headers) {
    return to$bytes("");
}


struct hpackQ_InflaterG_class;

struct hpackQ_Inflater {
    struct hpackQ_InflaterG_class *$class;
    nghttp2_hd_inflater *inflater;
};

B_NoneType hpackQ_InflaterD___init__(hpackQ_Inflater self) {
    nghttp2_hd_inflate_new(&self->inflater);
    return B_None;
}

B_NoneType hpackQ_InflaterD___del__(hpackQ_Inflater self) {
    nghttp2_hd_inflate_del(self->inflater);
    return B_None;
}

B_dict hpackQ_InflaterD_inflate(hpackQ_Inflater self, B_bytes compressed_headers) {
    B_dict ret;

    size_t inlen = (size_t)compressed_headers->nbytes;
    uint8_t *in = (uint8_t*)compressed_headers->str;
    
    nghttp2_ssize inflate_ret;
    nghttp2_nv nv_out;
    int inflate_flags;

    B_Hashable wit = (B_Hashable)B_HashableD_strG_witness;

    while (1) {
        inflate_ret = nghttp2_hd_inflate_hd3(self->inflater, &nv_out, &inflate_flags, in, inlen, 1);

	if (inflate_ret < 0) {
            // Need to handle these error cases more gracefully. Maybe an optional?
            break;
	}

	in += inflate_ret;
	inlen -= inflate_ret;

	if (inflate_flags & NGHTTP2_HD_INFLATE_EMIT) {
            char *name = acton_malloc(nv_out.namelen + 1);
	    char *value = acton_malloc(nv_out.valuelen + 1);

	    strncpy(name, (char*)nv_out.name, nv_out.namelen);
	    strncpy(value, (char*)nv_out.value, nv_out.valuelen);

	    B_dictD_setitem(ret, wit, name, value);
	}

	if (inflate_flags & NGHTTP2_HD_INFLATE_FINAL) {
            nghttp2_hd_inflate_end_headers(self->inflater);
            break;
	}

	if ((inflate_flags & NGHTTP2_HD_INFLATE_EMIT) == 0 &&
            inlen == 0) {
            break;
	}

    }

    return ret;
}
