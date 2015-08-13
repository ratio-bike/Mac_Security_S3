
EFI_STATUS
EFIAPI
Verify_SCAP (
	IN EFI_FIRMWARE_VOLUME_HEADER 	*fv,
	IN SCAP_Cert_block_t          	*scap
)
{
	if( scap->HashType != EFI_CERT_BLOCK_RSA_2048_SHA_256_GUID
		|| scap->scap_guid != APPLE_SCAP_GUID )
		return EFI_FAILURE;

	UINT8 sha256_hash[128] = SHA256(fv);
	if( !RSA_check(pubkey, sha256_hash, scap->Signature) )
		return EFI_FAILURE;

	gDS->ProcessFirmwareVolume(fv, fv_size, efi_handle);
	gDS->Dispatch();

	return EFI_SUCCESS;
    
    asdfef;
}

