/*
 * libinput_device.c
 *
 * Copyright (c) 2016 David Lechner <david@lechnology.com>
 * This file is part of the GRX graphics library.
 *
 * The GRX graphics library is free software; you can redistribute it
 * and/or modify it under some conditions; see the "copying.grx" file
 * for details.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <libinput.h>
#include <libudev.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <gio/gio.h>

#include <grx/libinput_device.h>

typedef struct {
    struct libinput_device *device;
} GrxLibinputDevicePrivate;

G_DEFINE_TYPE_WITH_CODE (GrxLibinputDevice,
    grx_libinput_device, G_TYPE_OBJECT,
    G_ADD_PRIVATE (GrxLibinputDevice))

/* Properties */

enum {
    PROP_0,
    PROP_NAME,
    PROP_SYSNAME,
    PROP_HAS_KEYBOARD,
    PROP_HAS_POINTER,
    PROP_HAS_TOUCH,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL };

const gchar *grx_libinput_device_get_name (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;

    return libinput_device_get_name (priv->device);
}

const gchar *grx_libinput_device_get_sysname (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;

    return libinput_device_get_sysname (priv->device);
}

gboolean grx_libinput_device_get_has_keyboard (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;

    return libinput_device_has_capability (priv->device,
                                           LIBINPUT_DEVICE_CAP_KEYBOARD);
}

gboolean grx_libinput_device_get_has_pointer (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;

    return libinput_device_has_capability (priv->device,
                                           LIBINPUT_DEVICE_CAP_POINTER);
}

gboolean grx_libinput_device_get_has_touch (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;

    return libinput_device_has_capability (priv->device,
                                           LIBINPUT_DEVICE_CAP_TOUCH);
}

static void
set_property (GObject *object, guint property_id, const GValue *value,
              GParamSpec *pspec)
{
    GrxLibinputDevice *self = GRX_LIBINPUT_DEVICE (object);

    switch (property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
get_property (GObject *object, guint property_id, GValue *value,
              GParamSpec *pspec)
{
    GrxLibinputDevice *self = GRX_LIBINPUT_DEVICE (object);
    GrxLibinputDevicePrivate *priv = self->private;

    switch (property_id) {
    case PROP_NAME:
        g_value_set_string (value, grx_libinput_device_get_name (self));
        break;
    case PROP_SYSNAME:
        g_value_set_string (value, grx_libinput_device_get_sysname (self));
        break;
    case PROP_HAS_KEYBOARD:
        g_value_set_boolean (value, grx_libinput_device_get_has_keyboard (self));
        break;
    case PROP_HAS_POINTER:
        g_value_set_boolean (value, grx_libinput_device_get_has_pointer (self));
        break;
    case PROP_HAS_TOUCH:
        g_value_set_boolean (value, grx_libinput_device_get_has_touch (self));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

/* class implementation */

static void finalize (GObject *object)
{
    GrxLibinputDevice *self = GRX_LIBINPUT_DEVICE (object);
    GrxLibinputDevicePrivate *priv = self->private;

    G_OBJECT_CLASS (grx_libinput_device_parent_class)->finalize (object);

    libinput_device_set_user_data (priv->device, NULL);
    priv->device = libinput_device_unref (priv->device);
}

static void
grx_libinput_device_class_init (GrxLibinputDeviceClass *klass)
{
    G_OBJECT_CLASS (klass)->set_property = set_property;
    G_OBJECT_CLASS (klass)->get_property = get_property;

    properties[PROP_NAME] =
        g_param_spec_string ("name",
                             "device name",
                             "Gets a descriptive name of the device",
                             NULL /* default value */,
                             G_PARAM_READABLE);
    properties[PROP_SYSNAME] =
        g_param_spec_string ("sysname",
                             "sysfs device name",
                             "Gets the sysfs name of the device",
                             NULL /* default value */,
                             G_PARAM_READABLE);
    properties[PROP_HAS_KEYBOARD] =
        g_param_spec_boolean ("has-keyboard",
                              "device has keyboard",
                              "Gets if the device has keyboard keys",
                              FALSE /* default value */,
                              G_PARAM_READABLE);
    properties[PROP_HAS_POINTER] =
        g_param_spec_boolean ("has-pointer",
                              "device has pointer",
                              "Gets if the device has a pointing device and/or buttons",
                              FALSE /* default value */,
                              G_PARAM_READABLE);
    properties[PROP_HAS_TOUCH] =
        g_param_spec_boolean ("has-touch",
                              "device has touch",
                              "Gets if the device has a touchscreen",
                              FALSE /* default value */,
                              G_PARAM_READABLE);
    g_object_class_install_properties (G_OBJECT_CLASS (klass),
                                       N_PROPERTIES,
                                       properties);

    G_OBJECT_CLASS (klass)->finalize = finalize;
}

static void
grx_libinput_device_init (GrxLibinputDevice *self)
{
    GrxLibinputDevicePrivate *priv =
        grx_libinput_device_get_instance_private (self);

    self->private = priv;
}

/* constructors */

/**
 * grx_libinput_device_new:
 * @device: the libinput device
 *
 * Creates a new instance of #GrxLibinputDevice
 *
 * Returns: the new instance
 */
GrxLibinputDevice *
grx_libinput_device_new (struct libinput_device *device)
{
    GrxLibinputDevice *instance = g_object_new (GRX_TYPE_LIBINPUT_DEVICE, NULL);
    GrxLibinputDevicePrivate *priv = instance->private;
    float matrix[6];

    g_return_val_if_fail (device != NULL, NULL);
    g_return_val_if_fail (libinput_device_get_user_data (device) == NULL, NULL);

    priv->device = libinput_device_ref (device);
    libinput_device_set_user_data (device, instance);

    if (libinput_device_config_calibration_get_default_matrix (device, matrix)) {
        libinput_device_config_calibration_set_matrix (device, matrix);
    }

    return instance;
}

/* methods */

/**
 * grx_libinput_device_uncalibrate:
 * @device: the device
 *
 * Resets the calibration of the device.
 *
 * This function is probably only useful to programs that are calibrating the
 * device and need to remove the existing calibration.
 *
 * Returns: %TRUE if this device can be calibrated and reseting the calibration
 * was successful.
 */
gboolean grx_libinput_device_uncalibrate (GrxLibinputDevice *device)
{
    GrxLibinputDevicePrivate *priv = device->private;
    float identity_matrix[6] = { 1, 0, 0, 0, 1, 0 };
    enum libinput_config_status ret;


    ret = libinput_device_config_calibration_set_matrix (priv->device,
                                                         identity_matrix);

    return ret == LIBINPUT_CONFIG_STATUS_SUCCESS;
}