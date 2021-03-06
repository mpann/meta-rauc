RAUC_KEYRING_FILE ??= "ca.cert.pem"
RAUC_KEYRING_URI ??= "file://${RAUC_KEYRING_FILE}"

SRC_URI_append = " \
  file://system.conf \
  ${RAUC_KEYRING_URI} \
  file://rauc-mark-good.service \
  "

SYSTEMD_PACKAGES += "${PN}-mark-good"
SYSTEMD_SERVICE_${PN}-mark-good = "rauc-mark-good.service"

inherit systemd

do_install () {
	autotools_do_install

        # Create rauc config dir
        # Warn if system configuration was not overwritten
        if ! grep -q "^[^#]" ${WORKDIR}/system.conf; then
                bbwarn "Please overwrite example system.conf with a project specific one!"
        fi
        install -d ${D}${sysconfdir}/rauc
        install -m 0644 ${WORKDIR}/system.conf ${D}${sysconfdir}/rauc/system.conf

        # Warn if CA file was not overwritten
        if ! grep -q "^[^#]" ${WORKDIR}/${RAUC_KEYRING_FILE}; then
                bbwarn "Please overwrite example ca.cert.pem with a project specific one, or set the RAUC_KEYRING_FILE variable with your file!"
        fi
        install -d ${D}${sysconfdir}/rauc/
        install -m 0644 ${WORKDIR}/${RAUC_KEYRING_FILE} ${D}${sysconfdir}/rauc/

        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/rauc-mark-good.service ${D}${systemd_unitdir}/system/
        sed -i -e 's!@BINDIR@!${bindir}!g' ${D}${systemd_unitdir}/system/*.service
}

PACKAGES =+ "${PN}-mark-good"

RDEPENDS_${PN} += "${@bb.utils.contains("PREFERRED_PROVIDER_virtual/bootloader", "barebox", "dt-utils-barebox-state", "",d)}"
RDEPENDS_${PN} += "${@bb.utils.contains("PREFERRED_PROVIDER_virtual/bootloader", "u-boot", "u-boot-fw-utils", "",d)}"

RRECOMMENDS_${PN}_append = " ${PN}-mark-good"

FILES_${PN}-mark-good = "${systemd_unitdir}/system/rauc-mark-good.service"

PACKAGECONFIG ??= "service network json"
