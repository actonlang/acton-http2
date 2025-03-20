#include <nghttp2/nghttp2.h>

#define DEFAULT_MAX_BUFLEN 4096

void http2Q_hpackQ___ext_init__() {
}

void *acton_malloc_cb(size_t size, void *mem_user_data) {
    //printf("acton_malloc_cb\n");
    return acton_malloc(size);
}

void *acton_calloc_cb(size_t nmemb, size_t size, void *mem_user_data) {
    //printf("acton_calloc_cb\n");
    return acton_calloc(nmemb, size);
}

void acton_free_cb(void *ptr, void *mem_user_data) {
    //printf("acton_free_cb\n");
    acton_free(ptr);
}

void *acton_realloc_cb(void *ptr, size_t size, void *mem_user_data) {
    //printf("acton_realloc_cb\n");
    return acton_realloc(ptr, size);
}

B_NoneType http2Q_hpackQ_DeflaterD__init_deflater (http2Q_hpackQ_Deflater self) {
    nghttp2_mem *mem = acton_malloc(sizeof(nghttp2_mem));
    *mem = (nghttp2_mem){NULL, acton_malloc_cb, acton_free_cb,
                         acton_calloc_cb, acton_realloc_cb};
    nghttp2_hd_deflater *deflater;
    nghttp2_hd_deflate_new2(&deflater, DEFAULT_MAX_BUFLEN, mem);
    //nghttp2_hd_deflate_new(&deflater, DEFAULT_MAX_BUFLEN);
    self->_deflater = toB_u64((unsigned long)deflater);
    return B_None;
}

B_NoneType http2Q_hpackQ_InflaterD__init_inflater (http2Q_hpackQ_Inflater self) {
    nghttp2_mem *mem = acton_malloc(sizeof(nghttp2_mem));
    *mem = (nghttp2_mem){NULL, acton_malloc_cb, acton_free_cb,
	                 acton_calloc_cb, acton_realloc_cb};
    nghttp2_hd_inflater *inflater;
    //nghttp2_hd_inflate_new2(&inflater, &mem);
    nghttp2_hd_inflate_new(&inflater);
    self->_inflater = toB_u64((unsigned long)inflater);
    return B_None;
}

B_bytes http2Q_hpackQ_DeflaterD_deflate(http2Q_hpackQ_Deflater self, B_dict headers) {
    nghttp2_hd_deflater *deflater = (nghttp2_hd_deflater*)fromB_u64(self->_deflater);

    B_IteratorD_dict_items iter = $NEW(B_IteratorD_dict_items, headers);
    B_tuple item;

    size_t numheaders = headers->numelements;

    nghttp2_nv *nvs = acton_calloc(numheaders, sizeof(nghttp2_nv));

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

    size_t buflen = nghttp2_hd_deflate_bound(deflater, nvs, numheaders);

    uint8_t* buf = acton_malloc(buflen+1);

    nghttp2_ssize deflate_ret = nghttp2_hd_deflate_hd2(deflater, buf, buflen, nvs, numheaders);

    if (deflate_ret < 0) {
        buf[0] = 0;
    }
    else {
        buf[buflen] = 0;
    }

    acton_free(nvs);

    return to$bytes((char*)buf);
}

B_dict http2Q_hpackQ_InflaterD_inflate(http2Q_hpackQ_Inflater self, B_bytes compressed_headers) {
    nghttp2_hd_inflater *inflater = (nghttp2_hd_inflater*)fromB_u64(self->_inflater);

    size_t inlen = (size_t)compressed_headers->nbytes;
    uint8_t *in = (uint8_t*)compressed_headers->str;

    nghttp2_ssize inflate_ret;
    nghttp2_nv nv_out;
    int inflate_flags;

    B_Hashable wit = (B_Hashable)B_HashableD_strG_witness;
    B_dict ret = $NEW(B_dict, wit, NULL, NULL);

    while (1) {
        inflate_ret = nghttp2_hd_inflate_hd3(inflater, &nv_out, &inflate_flags, in, inlen, 1);

	if (inflate_ret < 0) {
            // Need to handle these error cases more gracefully. Maybe an optional?
            break;
	}

	in += inflate_ret;
	inlen -= inflate_ret;

	if (inflate_flags & NGHTTP2_HD_INFLATE_EMIT) {
            char *name = acton_malloc(nv_out.namelen + 1);
	    char *value = acton_malloc(nv_out.valuelen + 1);

	    strncpy(name, (char*)nv_out.name, nv_out.namelen + 1);
	    strncpy(value, (char*)nv_out.value, nv_out.valuelen + 1);

	    B_dictD_setitem(ret, wit, to$str(name), to$str(value));
	}

	if (inflate_flags & NGHTTP2_HD_INFLATE_FINAL) {
            nghttp2_hd_inflate_end_headers(inflater);
            break;
	}

	if ((inflate_flags & NGHTTP2_HD_INFLATE_EMIT) == 0 &&
            inlen == 0) {
            break;
	}

    }

    return ret;
}
