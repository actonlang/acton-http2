#include <nghttp2/nghttp2.h>

#define DEFAULT_MAX_BUFLEN 4096

void hpackQ___ext_init__() {
}

/*struct hpackQ_DeflaterG_class;

struct hpackQ_Deflater {
    struct hpackQ_DeflaterG_class *$class;
    nghttp2_hd_deflater *deflater;
};*/

B_NoneType hpackQ_DeflaterD___init__(hpackQ_Deflater self) {
    nghttp2_hd_deflater *deflater;
    nghttp2_hd_deflate_new(&deflater, DEFAULT_MAX_BUFLEN);
    self->_deflater = deflater;
    return B_None;
}

B_NoneType hpackQ_DeflaterD___del__(hpackQ_Deflater self) {
    nghttp2_hd_deflate_del((nghttp2_hd_deflater*)self->_deflater);
    return B_None;
}

B_bytes hpackQ_DeflaterD_deflate(hpackQ_Deflater self, B_dict headers) {
    B_IteratorD_dict_items iter = $NEW(B_IteratorD_dict_items, headers);
    B_tuple item;

    nghttp2_hd_deflater *deflater = (nghttp2_hd_deflater*)self->_deflater;

    size_t numheaders = headers->numelements;

    nghttp2_nv *nvs = acton_calloc(sizeof(nghttp2_nv), numheaders);

    for (int i=0; i < numheaders; i++) {
        item = (B_tuple)iter->$class->__next__(iter);
        B_value key = item->components[0];
        B_value value = item->components[1];
        if (value && key->$class->$class_id == STR_ID &&
            value->$class->$class_id == STR_ID) {
            nvs[i].name = (uint8_t*)fromB_str((B_str)key);
	    nvs[i].namelen = ((B_str)key)->nbytes;
            nvs[i].value = (uint8_t*)fromB_str((B_str)value);
	    nvs[i].valuelen = ((B_str)value)->nbytes;
	    // Just set to 0 for now, work out NO_INDEX flag later
	    nvs[i].flags = 0;
	}
	else {
            // break on encountering invalid header? TBD
	    numheaders = i;
	    break;
	}
    }

    size_t buflen = nghttp2_hd_deflate_bound(self->deflater, nvs, numheaders);

    uint8_t* buf = acton_malloc(buflen+1);

    nghttp2_ssize deflate_ret = nghttp2_hd_deflate_hd2(self->deflater, buf, buflen, nvs, numheaders);

    if (deflate_ret < 0) {
        buf[0] = 0;
    }
    else {
        buf[buflen] = 0;
    }

    acton_free(nvs);

    return to$bytes((char*)buf);
}


/*struct hpackQ_InflaterG_class;

struct hpackQ_Inflater {
    struct hpackQ_InflaterG_class *$class;
    nghttp2_hd_inflater *inflater;
};*/

B_NoneType hpackQ_InflaterD___init__(hpackQ_Inflater self) {
    nghttp2_hd_inflater *inflater;
    nghttp2_hd_inflate_new(&inflater);
    self->_inflater = inflater;
    return B_None;
}

B_NoneType hpackQ_InflaterD___del__(hpackQ_Inflater self) {
    nghttp2_hd_inflate_del(self->_inflater);
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
